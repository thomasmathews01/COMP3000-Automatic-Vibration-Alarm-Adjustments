using System;
using System.Collections.Generic;
using System.Linq;
using BeranPlantProtech.Client;
using dataacquisition;
using dataacquisition.Model;

namespace DataAcquisitionApplication
{
    internal class Program
    {
        private const string HostAddress = "localhost";
        private const string HostPassword = "Level4";

        public static void Main(string[] args)
        {
            Console.WriteLine($"Application started successfully at: {DateTime.Now}");
            var connection = GetConnection();

            if (connection != null)
            {
                var sites = ExtractSites(connection).ToList();
                var successfullyExtractedMachines = sites.Select(x => AddMachinesToSite(connection, x)).Aggregate((x, y) => x && y);
                if (!successfullyExtractedMachines)
                    Console.WriteLine("Failed to extract basic machine information"); // TODO: Actually do something about this.

                RetrieveAcChannelDataForSites(connection, sites);
                StoreSitesInDatabase(sites);
                connection.Close();
                RetrieveAndStoreDataForSites(sites);
            }

            Console.WriteLine($"Finished data extraction at: {DateTime.Now}");
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

        private static IEnumerable<Site> ExtractSites(Connection connection)
        {
            try
            {
                var sites = connection.StationList().Select(x => new Site(x)).ToList();

                return sites;
            }
            catch (Exception e)
            {
                Console.WriteLine("Error whilst connecting.");
                Console.WriteLine(e);
            }

            return null;
        }

        private static bool AddMachinesToSite(Connection connection, Site site)
        {
            try
            {
                foreach (var machine in connection.UnitList(site.Name))
                    site.AddMachine(machine);

                return true;
            }
            catch (Exception e)
            {
                Console.WriteLine("Error whilst connecting.");
                Console.WriteLine(e);

                return false;
            }
        }

        private static void RetrieveAcChannelDataForSites(Connection connection, IEnumerable<Site> sites)
        {
            foreach (var site in sites) RequestAcChannelNamesForSite(connection, site);
        }

        private static void RequestAcChannelNamesForSite(Connection connection, Site site)
        {
            foreach (var machine in site.GetMachines())
                try
                {
                    var start = DateTime.Parse("21/09/1901");
                    var end = DateTime.Parse("21/09/2020");
                    var req = new DataRequest(connection, site.Name, machine.Name, start, end, uint.MaxValue);

                    for (var i = 0; i < req.ActiveACCount; ++i)
                        machine.Channels.Add(new Channel(req.GetACChannel(i).Name, req.GetACChannel(i).Units));
                }
                catch (Exception e)
                {
                    Console.WriteLine("Error whilst connecting.");
                    Console.WriteLine(e);
                }
        }

        private static void RetrieveAndStoreDataForSites(IEnumerable<Site> sites)
        {
            var start = DateTime.Parse("Wed, 21 Oct 2009 00:00:00 GMT");
            var end = start + TimeSpan.FromDays(1) - TimeSpan.FromMinutes(1);
            var db = new Database();

            foreach (var site in sites)
            {
                foreach (var machine in site.GetMachines().Where(x => x.Name == "TG 7" || x.Name == "TG 8"))
                {
                    for (var daysAdded = 0; daysAdded < 365; ++daysAdded)
                    {
                        foreach (var type in Enum.GetValues(typeof(DataRequest.AcType)).Cast<DataRequest.AcType>())
                            GetDataInRange(site.Name, machine.Name, type, start + TimeSpan.FromDays(daysAdded), end + TimeSpan.FromDays(daysAdded), db);
                    }
                }
            }
        }

        private static void GetDataInRange(string siteName, string machineName, DataRequest.AcType type, DateTime start, DateTime end, Database db)
        {
            var connection = GetConnection();
            try
            {
                int channels = GetACChannelsDuringTime(connection, siteName, machineName, start, end, uint.MaxValue);
                for (var i = 0; i < channels; ++i)
                {
                    var request = new DataRequest(connection, siteName, machineName, start, end, uint.MaxValue);
                    var someInt = request.RequestACMagData(i, type);
                    request.GetData();
                    db.AddDataToDatabase(machineName, request.GetACChannel(i).Name, (int) TimeSpan.FromDays(1).TotalSeconds / request.DataItem(someInt).Length, type, request.StartTime, request.DataItem(someInt));
                }
            }
            catch (Exception /* e */)
            {
                // Error handling? Whatever for?
            }
            finally
            {
                connection.Close();
            }
        }

        private static int GetACChannelsDuringTime(Connection connection, string siteName, string machineName, DateTime start, DateTime end, uint maxValue)
        {
            var activeAcCount = new DataRequest(connection, siteName, machineName, start, end, maxValue).ActiveACCount;
            return activeAcCount;
        }

        private static void StoreSitesInDatabase(IEnumerable<Site> sites)
        {
            var db = new Database();
            db.RunTableCreationScripts();
            foreach (var site in sites)
                db.AddSiteToDatabase(site);
        }
    }
}
