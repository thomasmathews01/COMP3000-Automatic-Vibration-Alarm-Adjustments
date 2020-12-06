using System.Collections.Generic;

namespace dataacquisition.Model
{
    public class Machine
    {
        public List<Channel> Channels = new List<Channel>();
        public string Name;

        public Machine(string machineName)
        {
            Name = machineName;
        }
    }
}
