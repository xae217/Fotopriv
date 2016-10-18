/*
    Uses OpenCV and it's Android port to load a pre trained Caffe model (GoogLeNet).
    More information on OpenCV can be found here:
    https://github.com/opencv

    Adapted from: https://github.com/alexkarargyris/Caffe_OpenCV_Android_App.git
 */

package com.xae18.fotopriv;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
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
import android.provider.MediaStore;
import android.support.v4.app.ActivityCompat;
import android.util.Log;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.view.View;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

public class MainActivity extends AppCompatActivity {

    private static final String MYLOG = "CopyToStorage" ;
    int REQUEST_CAMERA = 0, SELECT_FILE = 1;


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
        //copyToStorage();

        final Button buttonClassify = (Button) findViewById(R.id.button);
        buttonClassify.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                long tStart = System.currentTimeMillis();

                TextView tv = (TextView) findViewById(R.id.testTextView);

                tv.setText(NativeClass.getStringFromNative());

                long tEnd = System.currentTimeMillis();
                long tDelta = tEnd - tStart;
                double elapsedSeconds = tDelta / 1000.0;


                TextView clock = (TextView) findViewById(R.id.clock);
                clock.setText("Classified in "+Double.toString(elapsedSeconds) + " secs");
            }
        });



        final Button btnSelect = (Button) findViewById(R.id.button2);
        btnSelect.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                selectImage();
            }
        });
    }

    private void selectImage() {
        final CharSequence[] items = { "Take Photo", "Choose from Library", "Cancel" };

        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
        builder.setTitle("Add Photo!");
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

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (resultCode == Activity.RESULT_OK) {
            ImageView imgview = (ImageView) findViewById(R.id.image);
            Bitmap img;
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
                final int REQUIRED_SIZE = 200;
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

            String destFolder = getCacheDir().getAbsolutePath();

            FileOutputStream out = null;
            try {
                out = new FileOutputStream(destFolder + "/image.jpg");
                img.compress(Bitmap.CompressFormat.JPEG, 100, out);
                TextView tv = (TextView) findViewById(R.id.testTextView);
                tv.setText(destFolder + "/image.jpg");
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            }

        }

    }

    private void copyToStorage() {
        AssetManager assetManager = getResources().getAssets();
        String[] files = null;

        try {
            files = assetManager.list("Files");
        } catch (Exception e) {
            Log.d(MYLOG, "ERROR : " + e.toString());
        }

        for (int i = 0; i < files.length; i++) {
            InputStream in;
            OutputStream out;
            FileOutputStream fileOutStream;
            try {
                Log.d(MYLOG, "file names : " + files[i]);

                in = assetManager.open("Files/" + files[i]);
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
    }

    // API 23+ Requires to ask for permission dynamically.

    private static final int REQUEST_EXTERNAL_STORAGE = 1;
    private static String[] PERMISSIONS_STORAGE = {
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE
    };

    /**
     * Checks if the app has permission to write to device storage
     *
     * If the app does not has permission then the user will be prompted to grant permissions
     *
     * @param activity
     */
    public static void verifyStoragePermissions(Activity activity) {
        // check if there is write permission
        int permission = ActivityCompat.checkSelfPermission(activity, Manifest.permission.WRITE_EXTERNAL_STORAGE);

        if (permission != PackageManager.PERMISSION_GRANTED) {
            // we don't have permission so prompt the user
            ActivityCompat.requestPermissions(
                    activity,
                    PERMISSIONS_STORAGE,
                    REQUEST_EXTERNAL_STORAGE
            );
        }
    }
}
