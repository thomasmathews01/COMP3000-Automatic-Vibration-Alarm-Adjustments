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
        private const string ConnectionString = "Data Source=Z:\\VibrationData.db";

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
            using (var connection = new SQLiteConnection(ConnectionString))
            {
                connection.Open();
                foreach (var statement in _createTableStatements)
                {
                    var command = connection.CreateCommand();
                    command.CommandText = statement;
                    command.ExecuteNonQuery();
                }

                var transaction = connection.BeginTransaction();
                foreach (var type in Enum.GetValues(typeof(DataRequest.AcType)).Cast<DataRequest.AcType>())
                {
                    var command = new SQLiteCommand($"INSERT INTO types (type_name) VALUES (\"{type.ToString()}\");", connection);
                    command.ExecuteNonQuery();
                }

                transaction.Commit();
            }
        }

        public void AddSiteToDatabase(Site site)
        {
            using (var connection = new SQLiteConnection(ConnectionString))
            {
                connection.Open();
                var id = AddTopLevelSiteInformation(connection, site);
                foreach (var machine in site.GetMachines()) AddMachineToDatabase(connection, id, machine);
            }
        }

        private static void AddMachineToDatabase(SQLiteConnection connection, short siteId, Machine machine)
        {
            var machineId = AddTopLevelMachineInformationToDatabase(connection, siteId, machine);
            AddChannelsToDatabase(connection, machineId, machine.Channels);
        }

        private static void AddChannelsToDatabase(SQLiteConnection connection, short machineId, IEnumerable<Channel> machineChannels)
        {
            foreach (var channel in machineChannels)
            {
                var comm = connection.CreateCommand();
                comm.CommandText = $"INSERT INTO channels(channel_name, channel_units, machine_id) VALUES (\"{channel.Name}\", \"{channel.Units}\", {machineId})";
                comm.ExecuteNonQuery();
            }
        }

        private static short AddTopLevelMachineInformationToDatabase(SQLiteConnection connection, short id, Machine machine)
        {
            var addMachineCommand = connection.CreateCommand();

            addMachineCommand.CommandText = "INSERT INTO machines(machine_name, site_id) VALUES ($mname, $siteid)";
            addMachineCommand.Parameters.AddWithValue("$mname", machine.Name);
            addMachineCommand.Parameters.AddWithValue("$siteid", id);
            addMachineCommand.ExecuteNonQuery();

            var fetchIdCommand = connection.CreateCommand();
            fetchIdCommand.CommandText = $"SELECT machine_id from machines WHERE machine_name = \"{machine.Name}\"";

            using (var reader = fetchIdCommand.ExecuteReader())
            {
                if (reader.Read())
                    return reader.GetInt16(0);
            }

            return -1;
        }

        private short AddTopLevelSiteInformation(SQLiteConnection connection, Site site) // TODO: Refactor to maintain the connection as part of the class state
        {
            var comm = connection.CreateCommand();
            comm.CommandText = "INSERT INTO sites(site_name) VALUES ($name)";
            comm.Parameters.AddWithValue("$name", site.Name);
            comm.ExecuteNonQuery();

            var fetchIdCommand = connection.CreateCommand();
            fetchIdCommand.CommandText = $"SELECT site_id from SITES WHERE site_name = \"{site.Name}\"";

            using (var reader = fetchIdCommand.ExecuteReader())
            {
                if (reader.Read())
                    return reader.GetInt16(0);
            }

            return -1;
        }

        public void AddDataToDatabase(string machineName, string channelName, int secondsBetweenPoints, DataRequest.AcType type, DateTime requestStartTime, IEnumerable<float> data)
        {
            using (var connection = new SQLiteConnection(ConnectionString))
            {
                connection.Open();
                var storageTime = (int) requestStartTime.Subtract(new DateTime(1970, 1, 1, 0, 0, 0)).TotalSeconds;

                var transaction = connection.BeginTransaction();

                foreach (var point in data)
                {
                    var channelId = GetChannelIdFromChannelNameAndMachineId(channelName, GetMachineIdFromMachineName(machineName, connection), connection);
                    var typeId = GetTypeIdFromType(type, connection);
                    var insert = new SQLiteCommand($"INSERT INTO data (type_id, channel_id, time_since_epoch, value) VALUES ({typeId}, {channelId}, {storageTime}, {point})", connection);
                    insert.ExecuteNonQuery();

                    storageTime += secondsBetweenPoints;
                }

                transaction.Commit();
            }
        }

        private static int GetTypeIdFromType(DataRequest.AcType type, SQLiteConnection connection)
        {
            var command = new SQLiteCommand($"SELECT type_id FROM types where type_name = \"{type.ToString()}\"", connection);
            return int.Parse(command.ExecuteScalar().ToString());
        }

        private static int GetMachineIdFromMachineName(string machineName, SQLiteConnection connection)
        {
            var command = new SQLiteCommand($"SELECT machine_id FROM machines where machine_name = \"{machineName}\"", connection);
            return int.Parse(command.ExecuteScalar().ToString());
        }

        private static int GetChannelIdFromChannelNameAndMachineId(string channelName, int machineId, SQLiteConnection connection)
        {
            var command = new SQLiteCommand($"SELECT channel_id FROM channels where channel_name = \"{channelName}\" AND machine_id = {machineId}", connection);
            return int.Parse(command.ExecuteScalar().ToString());
        }
    }
}
