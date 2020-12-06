using System;
using System.Collections.Generic;
using BeranPlantProtech.Client;
using dataacquisition.Model;

namespace DataAcquisitionApplication
{
    public interface IDatabase
    {
        void RunTableCreationScripts();
        void AddSiteToDatabase(Site site);
        void StoreData(string siteName, string machineName, string channelName, int secondsBetweenPoints, DataRequest.AcType type, DateTime requestStartTime, IEnumerable<float> data);
        DateTime GetLastTimeForTypeOnChannel(string siteName, string machineName, string channelName, DataRequest.AcType type);
    }
}
