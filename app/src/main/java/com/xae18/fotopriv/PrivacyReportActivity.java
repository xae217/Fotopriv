package com.xae18.fotopriv;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.support.annotation.DrawableRes;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.StringTokenizer;

public class PrivacyReportActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_privacy_report);
        String result = getIntent().getExtras().getString("result");
        //Bitmap analyzedImage = getIntent().getParcelableExtra("bitmap");
        ImageView iv = (ImageView) findViewById(R.id.analyzedImage);
        ImageView iv2 =(ImageView) findViewById(R.id.feedback);
        BitmapFactory.Options options = new BitmapFactory.Options();
        final int REQUIRED_SIZE = 200;
        int scale = 1;
        while (options.outWidth / scale / 2 >= REQUIRED_SIZE
                && options.outHeight / scale / 2 >= REQUIRED_SIZE) {
            scale *= 2;
        }
        options.inSampleSize = scale;
        options.inJustDecodeBounds = false;
        //TODO: use proper cache directory
        iv.setImageBitmap(BitmapFactory.decodeFile("/data/data/com.xae18.fotopriv/cache/image.jpg", options));
        //iv.setImageBitmap(analyzedImage);
        if (result.equalsIgnoreCase("face not found|not")) {
            TextView found = (TextView) findViewById(R.id.fotoprivFound);
            found.setText("Fotopriv did not detect private content.");
            TextView title = (TextView) findViewById(R.id.reportTitle);
            title.setText("This image seems to be safe.");
            Bitmap bImage = BitmapFactory.decodeResource(this.getResources(), R.drawable.check);
            //iv2.setImageResource(R.drawable.check);
            iv2.setImageBitmap(bImage);
            //displayResult(result);
        }
        else {
            displayResult(result);
        }
    }

    private void displayResult(String result) {
        String analysisResult[] = result.split("\\|");
        for (int i = 0; i < analysisResult.length; i++) {
            Log.d("Result", analysisResult[i]);
        }
        TextView tv = (TextView) findViewById(R.id.prtv1);
        TextView tv2 = (TextView) findViewById(R.id.prtv2);
        TextView tv3 = (TextView) findViewById(R.id.prtv3);
        ArrayList<TextView> tvs = new ArrayList(3);
        tvs.add(tv);
        tvs.add(tv2);
        tvs.add(tv3);
        for(int i = 0; i < tvs.size(); i++) {
            if (i < analysisResult.length) {
                String r = analysisResult[i];
                if (r.equalsIgnoreCase("not") || r.equalsIgnoreCase("face not found")) r = "";
                tvs.get(i).setText(r);
            }
            else {
                tvs.get(i).setText("");
            }
        }
    }

}
