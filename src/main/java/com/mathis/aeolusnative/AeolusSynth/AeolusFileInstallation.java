// ----------------------------------------------------------------------------
//
//  Copyright (C) 2025 Thomas and Mathis Braschler <thomas.braschler@gmail.com>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------

package com.mathis.aeolusnative.AeolusSynth;


import android.content.Context;
import android.util.Log;

import com.mathis.aeolusnative.R;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.file.Path;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

/**
 * This class installs data contained in zip files in the raw ressource folder. This permits to have file and folder structures
 * available for Android
 */
public class AeolusFileInstallation {

    static {
        System.loadLibrary("AeolusAndroid");
    }

    /**
     * The application context, needed for absolute path generation
     */
    protected static Context mContext=null;

    /**
     * The root folder for storing app-private files
     */
    private static String privateStorageRoot ="";


    /**
     * Unhzips a zip archive and installs its content
     * See also https://stackoverflow.com/questions/11734084/how-to-unzip-file-that-that-is-not-in-utf8-format-in-java
     * @param target_folder_name Folder within the private data folder of the app to which to install the files
     * @param ressource_id Android Ressource ID of the zip file ressource
      */

    protected static void unzipinstallZipIfNotExists(String target_folder_name, int ressource_id) {

        Path folder_target_path = (new File(privateStorageRoot)).toPath().resolve(target_folder_name);
        File folder_target = folder_target_path.toFile();



        if (!folder_target.exists()) {
            folder_target.mkdirs();
            Log.v("mainActivity", "Installing to folder " + folder_target);

            ZipInputStream zipIs = new ZipInputStream(mContext.getResources().openRawResource(ressource_id));
            ZipEntry zEntry;

            while (true) {
                try {
                    if (!((zEntry = zipIs.getNextEntry()) != null)) break;
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }

                String localName = zEntry.getName();

                if (!localName.equals("")) {
                    if (zEntry.isDirectory()) {
                        folder_target_path.resolve(localName).toFile().mkdirs();
                    } else {
                        Log.v("mainActivity", localName);
                        byte[] tmp = new byte[4 * 1024];
                        FileOutputStream fos = null;
                        try {
                            fos = new FileOutputStream(folder_target_path.resolve(localName).toFile());
                        } catch (FileNotFoundException e) {
                            throw new RuntimeException(e);
                        }
                        int size = 0;
                        try{
                            while ((size = zipIs.read(tmp)) != -1) {
                                fos.write(tmp, 0, size);
                            }
                            fos.flush();
                            fos.close();}
                        catch (IOException e)
                        {
                            System.out.println("Probably java bug file not found due to encoding");
                        }
                    }
                }


            }
            try {
                zipIs.close();
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }

    }

    /**
     * Install the data in the raw zip files needed for Aeolus (instrument definitions, presets, stops)
     * The instrument definition and presets are in three version, Aeolus, Aeolus1, Aeolus2. By default,
     * and in this app, only the Aeolus folder is used
     * The stops are in the stops.zip file
     */

    public static void install_files() {


        unzipinstallZipIfNotExists("stops/Aeolus", R.raw.aeolus);
        unzipinstallZipIfNotExists("stops/Aeolus1", R.raw.aeolus1);
        unzipinstallZipIfNotExists("stops/Aeolus2", R.raw.aeolus2);
        unzipinstallZipIfNotExists("stops/stops", R.raw.stops);

    }

    /**
     * Set private storage root (app-specific data folder), including
     * transmission to the C implementation of Aeolus
     * @param newStorageRoot The app-specific, private data folder
     */
    protected static void setPrivateStorageRoot(String newStorageRoot)
    {
        privateStorageRoot =newStorageRoot;
        nativeSetStorageRoot(privateStorageRoot);


    }

    /**
     * Get the private data folder for this app from the context and set it.
     */

    public static void setStoragePrivateDefault()
    {
        if(mContext!=null)
        {
            setPrivateStorageRoot(mContext.getFilesDir().getAbsolutePath());
        }
    }

    /** Get the path to access to the default internal storage of the app on the
     * internal storage medium (app-private)
     */

    public static String getPrivateStorageRoot()
    {
        return privateStorageRoot;
    }


    /**
     * Initialize the storage locations (for now, private only)
     */
    public static void initStorageLocationsDefault()
    {

        setStoragePrivateDefault();

    }


    /** This function stores the context from the current activity
     *
     * @param context Context accessible the activity
     */
    public static void setContext(Context context) {
        mContext = context;
    }

    /**
     * Get the currently set context
     * @return Context, typically set by the main activity
     */

    public static Context getContext() {
        return mContext;
    }

    /**
     * Transmit the path to the app-private storage to the native implementation of Aeolus
     * @param thePath Absolute path the app-private storage
     */

    private static native void nativeSetStorageRoot(String thePath);

}
