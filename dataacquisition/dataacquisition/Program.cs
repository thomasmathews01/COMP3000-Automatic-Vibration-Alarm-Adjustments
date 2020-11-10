using System;
using dataacquisition;

namespace DataAcquisitionApplication
{
    internal class Program
    {
        // Example call: DataAcquisition.exe "Hunterston B" "TG 7" "01/01/2009" "01/01/2010"
        public static void Main(string[] args)
        {
            Console.WriteLine($"Application started successfully at: {DateTime.Now}");
            var e = new Extractor(args[0], args[1], args[2], args[3]);
            e.StartExtraction();
        }
    }
}
