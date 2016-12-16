package com.xae18.fotopriv;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.esafirm.imagepicker.features.ImagePicker;
import com.esafirm.imagepicker.model.Image;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;

public class AddUserActivity extends AppCompatActivity {


    static final int REQUEST_CODE_PICKER = 8;
    boolean selectedImages = false;
    String csvFileName = "fotopriv_images.csv";

    /**
     * onCreate method
     *
     * @param savedInstanceState Bundle
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_add_user);

        final Button buttonSelect = (Button) findViewById(R.id.selectImagesButton);
        buttonSelect.setOnClickListener(new View.OnClickListener() {

            public void onClick(View v) {
                ImagePicker.create(AddUserActivity.this) // Activity or Fragment
                        .start(REQUEST_CODE_PICKER);

            }

        });

        final Button buttonRegister = (Button) findViewById(R.id.registerUserButton);
        buttonRegister.setOnClickListener(new View.OnClickListener() {

            public void onClick(View v) {
                TextView tv = (TextView) findViewById(R.id.nameTF);
                if (selectedImages) {
                    String name = tv.getText().toString();
                    if (name.equalsIgnoreCase("")) {
                        tv.setError("Field must not be empty!");
                    } else {
                        String storagePath = getApplicationContext().getFilesDir().getAbsolutePath();
                        Log.d("File", storagePath + "/" + csvFileName);
                        File file = new File(storagePath + "/" + csvFileName);
                        if (file.exists()) {
                            Log.d("File", "FILE EXISTS!");
                            System.loadLibrary("MyLib");
                            NativeClass.registerUser(csvFileName, storagePath);
                            file.delete();

                            Intent intent = new Intent();
                            intent.putExtra("UserName", name);
                            setResult(10, intent);
                            finish();
                        }
                    }

                } else {
                    TextView tv2 = (TextView) findViewById(R.id.numOfSelcted);
                    tv2.setError("No images selected.");
                }
            }

        });
    }


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == REQUEST_CODE_PICKER && resultCode == RESULT_OK && data != null) {
            ArrayList<Image> images = (ArrayList<Image>) ImagePicker.getImages(data);
            createCsv(images);
        }
    }

    /**
     * Creates and saves a csv file with the path of images to be used to train the facial recognition model.
     *
     * @param images ArrayList of selected images.
     */
    private void createCsv(ArrayList<Image> images) {
        TextView tv = (TextView) findViewById(R.id.numOfSelcted);
        tv.setText("Selected " + images.size() + " images.");
        if (images.size() >= 5) {
            tv.setError(null);
            File file = new File(getApplicationContext().getFilesDir(), csvFileName);
            try {
                file.createNewFile();
                FileOutputStream fOut = new FileOutputStream(file);
                PrintWriter pw = new PrintWriter(fOut);
                for (Image i : images) {
                    // add '0' label to path. This would need to be changed to allow for more users
                    Log.d("Image Picker", i.getPath() + "; 0");
                    pw.println(i.getPath() + "; 0");
                }

                pw.flush();
                pw.close();
                fOut.close();

                selectedImages = true;
            } catch (IOException e) {
                Log.e("Exception", "File write failed: " + e.toString());
            }
        } else {
            tv.setError("At least 5 images need to be selected.");
        }

    }
}

