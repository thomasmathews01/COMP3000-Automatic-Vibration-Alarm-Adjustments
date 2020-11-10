using System;
using System.Collections.Generic;
using System.Data.SQLite;
using System.Linq;
using BeranPlantProtech.Client;
using dataacquisition.Model;
using DataAcquisitionApplication;

namespace dataacquisition
{
    public class Database : IDatabase
    {
        private readonly string _connectionString;
        private readonly SQLiteConnection _connection;

        public Database(string sourceLocation)
        {
            _connectionString = $"Data Source={sourceLocation}";
            _connection = new SQLiteConnection(_connectionString);
            _connection.Open();
        }

        private readonly string[] _createTableStatements =
        {
            "CREATE TABLE IF NOT EXISTS sites (site_id INTEGER PRIMARY KEY AUTOINCREMENT, site_name varchar(40));",
            "CREATE TABLE IF NOT EXISTS machines (machine_id INTEGER PRIMARY KEY AUTOINCREMENT, machine_name varchar(40), site_id INTEGER, FOREIGN KEY(site_id) REFERENCES sites(side_id));",
            "CREATE TABLE IF NOT EXISTS channels (channel_id INTEGER PRIMARY KEY AUTOINCREMENT, channel_name VARCHAR(40), channel_units VARCHAR(10), machine_id INTEGER, FOREIGN KEY(machine_id) REFERENCES machines(machine_id));",
            "CREATE TABLE IF NOT EXISTS types (type_id INTEGER PRIMARY KEY AUTOINCREMENT, type_name VARCHAR(30));",
            "CREATE TABLE IF NOT EXISTS data (type_id INTEGER, channel_id INTEGER, time_since_epoch INTEGER, value REAL, PRIMARY KEY (type_id, time_since_epoch, channel_id));"
        }; // TODO: Refactor all statements into their own namespace/class so they can be changed more easily.

        public void RunTableCreationScripts()
        {
            using (var connection = new SQLiteConnection(_connectionString))
            {
                connection.Open();
                foreach (var statement in _createTableStatements)
                {
                    var command = connection.CreateCommand();
                    command.CommandText = statement;
                    command.ExecuteNonQuery();
                }

                var transaction = connection.BeginTransaction();
                foreach (var type in Enum.GetValues(typeof(DataRequest.AcType)).Cast<DataRequest.AcType>().Where(TypeNotAlreadyInDatabase))
                {
                    var command = new SQLiteCommand($"INSERT INTO types (type_name) VALUES (\"{type.ToString()}\");", connection);
                    command.ExecuteNonQuery();
                }

                transaction.Commit();
            }
        }

        private bool TypeNotAlreadyInDatabase(DataRequest.AcType type)
        {
            var fetch = _connection.CreateCommand();
            fetch.CommandText = $"SELECT type_id from types WHERE type_name=\"{type.ToString()}\"";

            return fetch.ExecuteScalar() == null;
        }

        public void AddSiteToDatabase(Site site)
        {
            var id = AddTopLevelSiteInformation(site);
            foreach (var machine in site.GetMachines())
                AddMachineToDatabase(id, machine);
        }

        private long AddTopLevelSiteInformation(Site site)
        {
            var id = GetIdForSiteWithName(site.Name);
            if (id != -1)
                return id;

            var comm = _connection.CreateCommand();
            comm.CommandText = "INSERT INTO sites(site_name) VALUES ($name)";
            comm.Parameters.AddWithValue("$name", site.Name);
            comm.ExecuteNonQuery();

            return GetIdForSiteWithName(site.Name);
        }

        private long GetIdForSiteWithName(string name)
        {
            var fetchIdCommand = _connection.CreateCommand();
            fetchIdCommand.CommandText = $"SELECT site_id from SITES WHERE site_name = \"{name}\"";

            var x = fetchIdCommand.ExecuteScalar();
            return (long?) x ?? -1;
        }

        private void AddMachineToDatabase(long siteId, Machine machine)
        {
            var machineId = AddTopLevelMachineInformationToDatabase(siteId, machine);
            AddChannelsToDatabase(machineId, machine.Channels);
        }

        private long AddTopLevelMachineInformationToDatabase(long siteId, Machine machine)
        {
            var machineId = GetIdForMachineWithNameOnSiteWithId(machine.Name, siteId);
            if (machineId != -1)
                return machineId;

            var addMachineCommand = _connection.CreateCommand();
            addMachineCommand.CommandText = $"INSERT INTO machines(machine_name, site_id) VALUES (\"{machine.Name}\", {siteId})";
            addMachineCommand.ExecuteNonQuery();

            return GetIdForMachineWithNameOnSiteWithId(machine.Name, siteId);
        }

        private long GetIdForMachineWithNameOnSiteWithId(string name, long siteId)
        {
            var fetchIdCommand = _connection.CreateCommand();
            fetchIdCommand.CommandText = $"SELECT machine_id from machines WHERE site_id={siteId} AND machine_name = \"{name}\"";

            return (long?) fetchIdCommand.ExecuteScalar() ?? -1;
        }

        private void AddChannelsToDatabase(long machineId, IEnumerable<Channel> machineChannels)
        {
            foreach (var channel in machineChannels)
            {
                if (ChannelWithNameExistsOnMachineWithId(channel.Name, machineId))
                    break;

                var comm = _connection.CreateCommand();
                comm.CommandText = $"INSERT INTO channels(channel_name, channel_units, machine_id) VALUES (\"{channel.Name}\", \"{channel.Units}\", {machineId})";
                comm.ExecuteNonQuery();
            }
        }

        private bool ChannelWithNameExistsOnMachineWithId(string channelName, long machineId)
        {
            return GetIdForChannelWithNameAndMachineId(channelName, machineId) != -1;
        }

        private long GetIdForChannelWithNameAndMachineId(string channelName, long machineId)
        {
            var comm = _connection.CreateCommand();
            comm.CommandText = $"SELECT channel_id FROM channels WHERE channel_name = \"{channelName}\" AND machine_id = {machineId}";

            return (long?) comm.ExecuteScalar() ?? -1;
        }

        public DateTime GetLastTimeForTypeOnChannel(string siteName, string machineName, string channelName, DataRequest.AcType type)
        {
            var typeId = GetTypeIdFromType(type);
            var siteId = GetIdForSiteWithName(siteName);
            var machineId = GetIdForMachineWithNameOnSiteWithId(machineName, siteId);
            var channelId = GetIdForChannelWithNameAndMachineId(channelName, machineId);
            var fetchComm = _connection.CreateCommand();
            fetchComm.CommandText = $"SELECT MAX(time_since_epoch) from DATA WHERE type_id={typeId} AND channel_id={channelId}";

            var lastTime = new DateTime(1970, 1, 1, 0, 0, 0);
            var x = fetchComm.ExecuteScalar();
            if (!DBNull.Value.Equals(x))
                lastTime += TimeSpan.FromSeconds((long) x);

            return lastTime;
        }

        public void StoreData(string siteName, string machineName, string channelName, int secondsBetweenPoints, DataRequest.AcType type, DateTime requestStartTime, IEnumerable<float> data)
        {
            try
            {
                using (var connection = new SQLiteConnection(_connectionString))
                {
                    connection.Open();
                    var storageTime = (int) requestStartTime.Subtract(new DateTime(1970, 1, 1, 0, 0, 0)).TotalSeconds;

                    var transaction = connection.BeginTransaction();

                    foreach (var point in data)
                    {
                        var siteId = GetIdForSiteWithName(siteName);
                        var machineId = GetIdForMachineWithNameOnSiteWithId(machineName, siteId);
                        var channelId = GetIdForChannelWithNameAndMachineId(channelName, machineId);
                        var typeId = GetTypeIdFromType(type);
                        var insert = new SQLiteCommand($"INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES ({typeId}, {channelId}, {storageTime}, {point})", connection);
                        insert.ExecuteNonQuery();

                        storageTime += secondsBetweenPoints;
                    }

                    transaction.Commit();
                }
            }
            catch (Exception e)
            {
                // (string siteName, string machineName, string channelName, int secondsBetweenPoints, DataRequest.AcType type, DateTime requestStartTime, IEnumerable<float> data)
                Console.WriteLine($"Error storing 1 day of {type} data for {siteName} {machineName} {channelName} from {requestStartTime:dd/MM/yy}");
            }
        }

        private int GetTypeIdFromType(DataRequest.AcType type)
        {
            var command = new SQLiteCommand($"SELECT type_id FROM types where type_name = \"{type.ToString()}\"", _connection);
            return int.Parse(command.ExecuteScalar().ToString());
        }
    }
}
