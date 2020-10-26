using dataacquisition.Model;

namespace DataAcquisitionApplication
{
    public interface IDatabase
    {
        void RunTableCreationScripts();
        void AddSiteToDatabase(Site site);
    }
}
