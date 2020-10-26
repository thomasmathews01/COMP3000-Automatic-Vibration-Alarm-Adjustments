using System.Collections.Generic;
using System.Data.SQLite;
using dataacquisition.Model;
using DataAcquisitionApplication;

namespace dataacquisition
{
    public class Database : IDatabase
    {
        private const string ConnectionString = "Data Source=C:\\temp\\VibrationData.db";

        private readonly string[] _createTableStatements =
        {
            "CREATE TABLE IF NOT EXISTS sites (site_id INTEGER PRIMARY KEY AUTOINCREMENT, site_name varchar(40));",
            "CREATE TABLE IF NOT EXISTS machines (machine_id INTEGER PRIMARY KEY AUTOINCREMENT, machine_name varchar(40), site_id INTEGER, FOREIGN KEY(site_id) REFERENCES sites(side_id));",
            "CREATE TABLE IF NOT EXISTS channels (channel_id INTEGER PRIMARY KEY AUTOINCREMENT, channel_name VARCHAR(40), channel_units VARCHAR(10), machine_id INTEGER, FOREIGN KEY(machine_id) REFERENCES machines(machine_id));",
            "CREATE TABLE IF NOT EXISTS data_types (data_type_id INTEGER PRIMARY KEY AUTOINCREMENT, type_value INTEGER, type_name VARCHAR(25));",
            "CREATE TABLE IF NOT EXISTS data_points (point_time INTEGER, data_type_id INTEGER, channel_id INTEGER, value real, PRIMARY KEY(point_time, data_type_id, channel_id), FOREIGN KEY(data_type_id) REFERENCES data_types(data_type_id),FOREIGN KEY(channel_id) REFERENCES channels(channel_id));"
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
    }
}
