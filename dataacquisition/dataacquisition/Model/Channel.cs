namespace dataacquisition.Model
{
    public class Channel
    {
        public Channel(string name, string units)
        {
            Name = name;
            Units = units;
        }

        public string Name { get; set; }
        public string Units { get; set; }
    }
}
