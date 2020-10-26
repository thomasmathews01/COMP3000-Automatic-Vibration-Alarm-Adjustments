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
            Console.WriteLine("Application started successfully");

            if (CanConnect())
            {
                var sites = ExtractSites().ToList();
                var successfullyExtractedMachines = sites.Select(AddMachinesToSite).Aggregate((x, y) => x && y);
                if (!successfullyExtractedMachines)
                    Console.WriteLine("Failed to extract basic machine information"); // TODO: Actually do something about this.

                RetrieveAcChannelDataForSites(sites);
                StoreSitesInDatabase(sites);
            }

            Console.ReadKey();
        }

        private static bool CanConnect()
        {
            var connection = new Connection();

            try
            {
                connection.ConnectTo(HostAddress, HostPassword);
                connection.Close();
                return true;
            }
            catch (Exception e)
            {
                Console.WriteLine("Error whilst connecting.");
                Console.WriteLine(e);
                connection.Close();
                return false;
            }
        }

        private static IEnumerable<Site> ExtractSites()
        {
            var connection = new Connection();
            try
            {
                connection.ConnectTo(HostAddress, HostPassword);
                var sites = connection.StationList().Select(x => new Site(x)).ToList();
                connection.Close();
                return sites;
            }
            catch (Exception e)
            {
                Console.WriteLine("Error whilst connecting.");
                Console.WriteLine(e);
                connection.Close();
            }

            return null;
        }

        private static bool AddMachinesToSite(Site site)
        {
            var connection = new Connection();
            try
            {
                connection.ConnectTo(HostAddress, HostPassword);

                foreach (var machine in connection.UnitList(site.Name))
                    site.AddMachine(machine);

                connection.Close();
                return true;
            }
            catch (Exception e)
            {
                Console.WriteLine("Error whilst connecting.");
                Console.WriteLine(e);
                connection.Close();

                return false;
            }
        }

        private static void RetrieveAcChannelDataForSites(IEnumerable<Site> sites)
        {
            foreach (var site in sites) RequestAcChannelNamesForSite(site);
        }

        private static void RequestAcChannelNamesForSite(Site site)
        {
            var connection = new Connection();
            foreach (var machine in site.GetMachines())
                try
                {
                    connection.ConnectTo(HostAddress, HostPassword);
                    var start = DateTime.Parse("21/09/1901");
                    var end = DateTime.Parse("21/09/2020");
                    var req = new DataRequest(connection, site.Name, machine.Name, start, end, uint.MaxValue);

                    for (var i = 0; i < req.ActiveACCount; ++i)
                        machine.Channels.Add(new Channel(req.GetACChannel(i).Name, req.GetACChannel(i).Units));

                    connection.Close();
                }
                catch (Exception e)
                {
                    Console.WriteLine("Error whilst connecting.");
                    Console.WriteLine(e);
                    connection.Close();
                }
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
