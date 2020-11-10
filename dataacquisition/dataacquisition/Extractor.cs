using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using BeranPlantProtech.Client;
using dataacquisition.Model;
using DataAcquisitionApplication;

namespace dataacquisition
{
    public class Extractor
    {
        private readonly string _siteName;
        private readonly string _machineName;
        private readonly DateTime _startTime;
        private readonly DateTime _endTime;

        private const string HostAddress = "localhost";
        private const string HostPassword = "Level4";

        private List<Site> _sites = new List<Site>();
        private readonly IDatabase _database = new Database("Z:\\VibrationData.db");

        private static readonly DataRequest.AcType[] _defaultTypes =
        {
            DataRequest.AcType.AC_SUBSYNC_FREQ,
            DataRequest.AcType.AC_PEAK_TO_PEAK,
            DataRequest.AcType.AC_NONSYNC,
            DataRequest.AcType.AC_RMS,
            DataRequest.AcType.AC_ORDER1,
            DataRequest.AcType.AC_RMS_MINUS_SUM,
            DataRequest.AcType.AC_SUBSYNC,
            DataRequest.AcType.AC_ORDER3,
            DataRequest.AcType.AC_ORDER2,
            DataRequest.AcType.AC_ORDER4
        };

        /**
         * Usage notes:
         * Assumes that the configuration remained constant
         * Does not account for extra recording fidelity for short periods.
         */
        public Extractor(string siteName, string machineName, string startTime, string endTime)
        {
            _siteName = siteName;
            _machineName = machineName;
            _startTime = DateTime.Parse(startTime, new CultureInfo("en-GB"));
            _endTime = DateTime.Parse(endTime, new CultureInfo("en-GB"));
        }

        public void StartExtraction()
        {
            Console.WriteLine($"Extracting information for site: {_siteName}, machine: {_machineName} between {_startTime} and {_endTime}");
            _database.RunTableCreationScripts();

            GetSites();
            AddMachineInfoToSites();
            RetrieveAcChannelDataForSites();
            StoreSitesInDatabase();

            RetrieveAndStoreDataForSites();

            Console.WriteLine($"Finished data extraction at: {DateTime.Now}");
        }

        private void GetSites()
        {
            var connection = GetConnection();
            _sites = connection.StationList().Select(x => new Site(x)).ToList();
            connection.Close();
        }

        private void AddMachineInfoToSites()
        {
            var connection = GetConnection();

            foreach (var site in _sites)
            {
                foreach (var machine in connection.UnitList(site.Name))
                    site.AddMachine(machine);
            }

            connection.Close();
        }

        private void RetrieveAcChannelDataForSites()
        {
            var connection = GetConnection();
            foreach (var site in _sites) RequestAcChannelNamesForSite(connection, site);
            connection.Close();
        }

        private void RequestAcChannelNamesForSite(Connection connection, Site site)
        {
            foreach (var machine in site.GetMachines())
                try
                {
                    var req = new DataRequest(connection, site.Name, machine.Name, _startTime, _endTime, uint.MaxValue);

                    for (var i = 0; i < req.ActiveACCount; ++i)
                        machine.Channels.Add(new Channel(req.GetACChannel(i).Name, req.GetACChannel(i).Units));
                }
                catch (Exception e)
                {
                    Console.WriteLine("Error whilst connecting.");
                    Console.WriteLine(e);
                }
        }

        private void StoreSitesInDatabase()
        {
            foreach (var site in _sites)
                _database.AddSiteToDatabase(site);
        }

        private void RetrieveAndStoreDataForSites()
        {
            var connection = GetConnection();
            foreach (var type in _defaultTypes)
            {
                var startOfData = GetStartOfData(connection, _siteName, _machineName, _startTime, _endTime, uint.MaxValue);
                var daysAfterInputStart = Math.Floor((startOfData - _startTime).TotalDays) > 0 ? Math.Floor((startOfData - _startTime).TotalDays) : 0;
                for (var daysAdded = daysAfterInputStart; daysAdded < (_endTime - _startTime).TotalDays; ++daysAdded)
                {
                    var requestPeriodStart = _startTime + TimeSpan.FromDays(daysAdded);
                    var requestPeriodEnd = _startTime + TimeSpan.FromDays(daysAdded + 1);

                    var channels = GetACChannelsDuringTime(connection, _siteName, _machineName, requestPeriodStart, requestPeriodEnd, uint.MaxValue);
                    for (var i = 0; i < channels; ++i)
                    {
                        try
                        {
                            Console.WriteLine($"{DateTime.Now:hh:mm:ss} Requesting {type} data for channel {i + 1} between {requestPeriodStart:dd/MM/yy} & {requestPeriodEnd:dd/MM/yy}.");
                            var request = new DataRequest(connection, _siteName, _machineName, requestPeriodStart, requestPeriodEnd, uint.MaxValue);
                            var timeOfLastStoredPoint = _database.GetLastTimeForTypeOnChannel(_siteName, _machineName, request.GetACChannel(i).Name, type);
                            if (timeOfLastStoredPoint >= request.StartTime || (request.StartTime - requestPeriodStart > TimeSpan.FromDays(1)))
                                break;

                            var dataPointer = request.RequestACMagData(i, type);

                            request.GetData();

                            Console.WriteLine($"{DateTime.Now:hh:mm:ss} Storing {type} data for channel {i + 1} between {requestPeriodStart:dd/MM/yy} & {requestPeriodEnd:dd/MM/yy}.");
                            var values = request.DataItem(dataPointer).Where(x => !DataRequest.IsSeparatorValue(x)).ToArray();
                            var timeBetweenValues = (int) TimeSpan.FromDays(1).TotalSeconds / values.Length;

                            _database.StoreData(_siteName, _machineName, request.GetACChannel(i).Name, timeBetweenValues, type, request.StartTime, values);
                        }
                        catch (Exception e)
                        {
                            Console.WriteLine($"{DateTime.Now:hh:mm:ss} Exception thrown when requesting {type} data for channel {i + 1} between {requestPeriodStart:dd/MM/yy} & {requestPeriodEnd:dd/MM/yy}. Continuing...");
                            Console.WriteLine(e);
                        }
                    }
                }
            }

            connection.Close();
        }

        private DateTime GetStartOfData(Connection connection, string siteName, string machineName, DateTime start, DateTime end, uint maxValue)
        {
            return new DataRequest(connection, siteName, machineName, start, end, maxValue).StartTime;
        }

        private static int GetACChannelsDuringTime(Connection connection, string siteName, string machineName, DateTime start, DateTime end, uint maxValue)
        {
            var activeAcCount = new DataRequest(connection, siteName, machineName, start, end, maxValue).ActiveACCount;
            return activeAcCount;
        }

        private static Connection GetConnection()
        {
            var connection = new Connection();

            try
            {
                connection.ConnectTo(HostAddress, HostPassword);
                return connection;
            }
            catch (Exception e)
            {
                Console.WriteLine("Error whilst connecting.");
                Console.WriteLine(e);
                connection.Close();

                return null;
            }
        }
    }
}
