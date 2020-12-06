using System.Collections.Generic;

namespace dataacquisition.Model
{
    public class Site
    {
        private readonly List<Machine> _machines = new List<Machine>();

        public Site(string siteName)
        {
            Name = siteName;
        }

        public string Name { get; set; }

        public void AddMachine(string machineName)
        {
            _machines.Add(new Machine(machineName));
        }

        public IEnumerable<Machine> GetMachines()
        {
            return _machines;
        }
    }
}
