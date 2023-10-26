




#include <iostream>

#include "gdal_priv.h"


int main(int argc, char** argv)
{
    const char* file = NULL;    
    bool hasGeoTransform = false;
    bool hasGCPs = false;

    // usage: geotiffCorners <file>
    // This program will print the corner latitudes and longitudes corners of a geotiff image file
    // It first checks if the geotransform is available, if not it checks for Ground Control Points
    // If no geotransform or GCPs are found, the program will exit
    if (argc != 2)
    {
        std::cout << "usage: geotiffCorners <file>" << std::endl;
        return 1;
    }
    else
    {
        file = argv[1];
    }

    GDALAllRegister();

    // Open the geotiff file
    GDALDataset* dataset = (GDALDataset*) GDALOpen(file, GA_ReadOnly);

    if (dataset == NULL)
    {
        std::cout << "Error opening file: " << file << std::endl;
        return 1;
    }

    // Check if the file is a valid geotiff
    if (dataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME) == NULL)
    {
        std::cout << "File is not a valid geotiff\n";
        return 1;
    }

    // Check if the file has a geotransform a geotransform can be used to convert pixel coordinates to lat/lon
    // Get the geotransform
    double geoTransform[6];
    
    // Check if the geotransform was successfully retrieved
    if (dataset->GetGeoTransform(geoTransform) == CE_None)
    {
        hasGeoTransform = true;
    }
    else
    {
        std::cout << "No geotransform found\n";
    }

    if (!hasGeoTransform)
    {
        // Search for GCPs
        if (dataset->GetGCPCount() > 0)
        {
            hasGCPs = true;
        }
        else
        {
            std::cout << "No GCPs found\n";
        }
    }

    if (hasGeoTransform)
    {
        // Get the width and height of the image
        int width = dataset->GetRasterXSize();
        int height = dataset->GetRasterYSize();

        // Get the corner coordinates, convert to lat/lon
        double lat = 0.0;
        double lon = 0.0;

        // The geoTransform is a 6 element array
        // X_geo = GT(0) + X_pixel * GT(1) + Y_line * GT(2)
        // Y_geo = GT(3) + X_pixel * GT(4) + Y_line * GT(5)

        // Upper left
        lon = geoTransform[0] + 0.5 * geoTransform[1] + 0.5 * geoTransform[2];
        lat = geoTransform[3] + 0.5 * geoTransform[4] + 0.5 * geoTransform[5];

        printf("%2.15lf %2.15lf\n", lon, lat);

        // Upper right
        lon = geoTransform[0] + (width-0.5) * geoTransform[1] + 0.5 * geoTransform[2];
        lat = geoTransform[3] + (width-0.5) * geoTransform[4] + 0.5 * geoTransform[5];

        printf("%2.15lf %2.15lf\n", lon, lat);

        // Lower right
        lon = geoTransform[0] + (width-0.5) * geoTransform[1] + (height-0.5) * geoTransform[2];
        lat = geoTransform[3] + (width-0.5) * geoTransform[4] + (height-0.5) * geoTransform[5];

        printf("%2.15lf %2.15lf\n", lon, lat);

        // Lower left
        lon = geoTransform[0] + 0.5 * geoTransform[1] + (height-0.5) * geoTransform[2];
        lat = geoTransform[3] + 0.5 * geoTransform[4] + (height-0.5) * geoTransform[5];

        printf("%2.15lf %2.15lf\n", lon, lat);
    }
    else if (hasGCPs)
    {
        // Must be at least 3 GCPs, otherwise we cant create a geoTransform array from them.
        if (dataset->GetGCPCount() < 3)
        {
            std::cout << "Not enough GCPs to create a geotransform\n";
            return 1;
        }

        // Get the width and height of the image
        int width = dataset->GetRasterXSize();
        int height = dataset->GetRasterYSize();

        // Create a geotransform array from the GCPs
        double geoTransform[6];

        // Get the GCPs
        const GDAL_GCP* gcps = dataset->GetGCPs();

        // Create the geotransform
        int res = GDALGCPsToGeoTransform(dataset->GetGCPCount(), gcps, geoTransform, 1);
        if (res != CE_None)
        {
            std::cout << "Error creating geotransform from GCPs\n";
            return 1;
        }

        // Upper left
        double lon = geoTransform[0] + 0.5 * geoTransform[1] + 0.5 * geoTransform[2];
        double lat = geoTransform[3] + 0.5 * geoTransform[4] + 0.5 * geoTransform[5];

        printf("%2.15lf %2.15lf\n", lon, lat);

        // Upper right
        lon = geoTransform[0] + (width-0.5) * geoTransform[1] + 0.5 * geoTransform[2];
        lat = geoTransform[3] + (width-0.5) * geoTransform[4] + 0.5 * geoTransform[5];

        printf("%2.15lf %2.15lf\n", lon, lat);

        // Lower right
        lon = geoTransform[0] + (width-0.5) * geoTransform[1] + (height-0.5) * geoTransform[2];
        lat = geoTransform[3] + (width-0.5) * geoTransform[4] + (height-0.5) * geoTransform[5];

        printf("%2.15lf %2.15lf\n", lon, lat);

        // Lower left
        lon = geoTransform[0] + 0.5 * geoTransform[1] + (height-0.5) * geoTransform[2];
        lat = geoTransform[3] + 0.5 * geoTransform[4] + (height-0.5) * geoTransform[5];

        printf("%2.15lf %2.15lf\n", lon, lat);        
    }
    else
    {
        std::cout << "No geotransform or GCPs found\n";
        return 1;
    }

    return 0;
}




