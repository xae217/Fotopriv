package com.xae18.fotopriv;

/**
 * Created by xae18 on 10/17/16.
 */

/**
 * NativeClass
 * Includes methods for JNI calls.
 */
public class NativeClass {
    /**
     * Gets the result from the image analysis as a string.
     * @param isRegistered Shows if the user has registered for face recognition or not.
     * @param storagePath Path to internal storage.
     * @return String object with analysis result.
     */
    public native static String getStringFromNative(int isRegistered, String storagePath);

    /**
     * Registers a user for facial recognition.
     * @param csvPath Path to csv file to be use to train the face recognition model.
     * @param storagePath Path to internal storage.
     * @return Returns an integer.
     */
    public native static int registerUser(String csvPath, String storagePath);
}
