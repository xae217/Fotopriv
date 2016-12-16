/*
    Uses OpenCV and it's Android port to load a pre trained Caffe model (GoogLeNet).
    More information on OpenCV can be found here:
    https://github.com/opencv

    Adapted from: https://github.com/alexkarargyris/Caffe_OpenCV_Android_App.git
 */

package com.xae18.fotopriv;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

public class MainActivity extends AppCompatActivity {

    //For camera permission
    private static final int REQUEST_CAMERA = 0, SELECT_FILE = 1, REQUEST_EXTERNAL_STORAGE = 2;

    //For storage permissions
    private static String[] FOTOPRIV_PERMISSIONS = {
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.CAMERA // and camera as well
    };

    static {
        System.loadLibrary("MyLib");
        System.loadLibrary("opencv_java3");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView tv = (TextView) findViewById(R.id.testTextView);
        verifyStoragePermissions(this);
        tv.setText("");

        final String storagePath = copyToStorage();

        //copy sample image to cache
        Bitmap sampleImg = BitmapFactory.decodeResource(getResources(), R.drawable.default_image);
        copyToCache(sampleImg);

        final Button buttonClassify = (Button) findViewById(R.id.classify);
        buttonClassify.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                File file = new File(storagePath + "/fotopriv.yml");
                Log.d("File", storagePath + "/fotopriv.yml");
                if (file.exists()) {
                    analyzeImage(1, storagePath);
                } else {
                    alertUser(storagePath);
                }
            }
        });


        final Button btnSelect = (Button) findViewById(R.id.loadImage);
        btnSelect.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                selectImage();
            }
        });

        final Button buttonRegister = (Button) findViewById(R.id.registerUser);
        buttonRegister.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this, RegisterUserActivity.class);
                MainActivity.this.startActivity(intent);
            }
        });
    }

    private void selectImage() {
        final CharSequence[] items = {"Take Photo", "Choose from Library", "Cancel"};

        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
        builder.setTitle("Load Photo!");
        builder.setItems(items, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int item) {
                if (items[item].equals("Take Photo")) {
                    Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
                    startActivityForResult(intent, REQUEST_CAMERA);
                } else if (items[item].equals("Choose from Library")) {
                    Intent intent = new Intent(
                            Intent.ACTION_PICK,
                            android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
                    intent.setType("image/*");
                    startActivityForResult(
                            Intent.createChooser(intent, "Select File"),
                            SELECT_FILE);
                } else if (items[item].equals("Cancel")) {
                    dialog.dismiss();
                }
            }
        });
        builder.show();
    }

    /**
     * Makes a JNI call to perform privacy detection on an image.
     *
     * @param flag        Shows if the user is registered. Facial recognition is disabled/enabled.
     * @param storagePath Path to internal storage.
     */
    private void analyzeImage(int flag, String storagePath) {
        Log.d("File", "FR model exists");
        String analyzisResult = NativeClass.getStringFromNative(flag, storagePath);
        Intent intent = new Intent(MainActivity.this, PrivacyReportActivity.class);
        intent.putExtra("result", analyzisResult);
        MainActivity.this.startActivity(intent);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        Bitmap img;
        if (resultCode == Activity.RESULT_OK) {
            ImageView imgview = (ImageView) findViewById(R.id.image);

            if (requestCode == REQUEST_CAMERA) {
                img = (Bitmap) data.getExtras().get("data");
                imgview.setImageBitmap(img);
            } else {
                Uri selectedImageUri = data.getData();
                String[] projection = {MediaStore.MediaColumns.DATA};
                Cursor cursor = managedQuery(selectedImageUri, projection, null, null,
                        null);
                int column_index = cursor.getColumnIndexOrThrow(MediaStore.MediaColumns.DATA);
                cursor.moveToFirst();

                String selectedImagePath = cursor.getString(column_index);

                BitmapFactory.Options options = new BitmapFactory.Options();
                options.inJustDecodeBounds = true;
                BitmapFactory.decodeFile(selectedImagePath, options);
                final int REQUIRED_SIZE = 224;
                int scale = 1;
                while (options.outWidth / scale / 2 >= REQUIRED_SIZE
                        && options.outHeight / scale / 2 >= REQUIRED_SIZE) {
                    scale *= 2;
                }
                options.inSampleSize = scale;
                options.inJustDecodeBounds = false;
                img = BitmapFactory.decodeFile(selectedImagePath, options);

                imgview.setImageBitmap(img);
            }
            copyToCache(img);
        }
    }


    /**
     * Alert the user if facial recognition is disabled.
     *
     * @param storagePath
     */
    private void alertUser(final String storagePath) {
        AlertDialog.Builder builder1 = new AlertDialog.Builder(this);
        builder1.setMessage("Face recognition is disabled. Please register to let Fotopriv recognize you.");
        builder1.setCancelable(true);

        builder1.setPositiveButton(
                "Continue",
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        analyzeImage(0, storagePath);
                        dialog.cancel();
                    }
                });

        builder1.setNegativeButton(
                "Cancel",
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.cancel();
                    }
                });

        AlertDialog alert11 = builder1.create();
        alert11.show();
    }

    /**
     * Copies image to cache
     *
     * @param img Image to be copied to cache storage.
     */
    private void copyToCache(Bitmap img) {
        String destFolder = getCacheDir().getAbsolutePath();

        FileOutputStream out = null;
        try {
            out = new FileOutputStream(destFolder + "/image.jpg");
            img.compress(Bitmap.CompressFormat.JPEG, 100, out);
            TextView tv = (TextView) findViewById(R.id.testTextView);
            //tv.setText(destFolder + "/image.jpg");
            tv.setText("Image loaded");
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
    }

    /**
     * Copies needed resources to storage:
     * Caffe model and prototxt.
     * haarcascade for eye and frontal face recognition.
     * flandmark_model.dat
     * synset_words.txt
     *
     * @return Returns the internal storage path.
     */
    private String copyToStorage() {
        final String MYLOG = "CopyToStorage"; // for logging..
        AssetManager assetManager = getResources().getAssets();
        String[] files = null;
        String storagePath = getApplicationContext().getFilesDir().getAbsolutePath();
        try {
            files = assetManager.list("files");
        } catch (Exception e) {
            Log.d(MYLOG, "ERROR : " + e.toString());
        }

        for (int i = 0; i < files.length; i++) {
            InputStream in;
            OutputStream out;
            FileOutputStream fileOutStream;
            try {
                Log.d(MYLOG, "file names : " + files[i]);

                in = assetManager.open("files/" + files[i]);
                out = new FileOutputStream(getApplicationContext().getFilesDir() + files[i]);
                File file = new File(getApplicationContext().getFilesDir(), files[i]);

                byte[] buffer = new byte[65536 * 2];
                int read;
                while ((read = in.read(buffer)) != -1) {
                    out.write(buffer, 0, read);
                }

                in.close();
                out.flush();
                fileOutStream = new FileOutputStream(file);
                fileOutStream.write(buffer);
                out.close();

                Log.d(MYLOG, "File Copied in storage");
            } catch (Exception e) {
                Log.d(MYLOG, "ERROR: " + e.toString());
            }
        }
        return storagePath;
    }


    /**
     * Checks if the app has permission to write to device storage/
     * If the app does not has permission then the user will be prompted to grant permissions.
     *
     * @param activity Current Activity.
     */
    public static void verifyStoragePermissions(Activity activity) {
        // check if there is write permission
        int permission = ActivityCompat.checkSelfPermission(activity, Manifest.permission.WRITE_EXTERNAL_STORAGE);

        if (permission != PackageManager.PERMISSION_GRANTED) {
            // we don't have permission so prompt the user
            ActivityCompat.requestPermissions(
                    activity,
                    FOTOPRIV_PERMISSIONS,
                    REQUEST_EXTERNAL_STORAGE
            );
        }
    }

}
