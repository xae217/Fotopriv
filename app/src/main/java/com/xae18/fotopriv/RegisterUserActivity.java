package com.xae18.fotopriv;

import android.content.Intent;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.PopupMenu;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.esafirm.imagepicker.features.ImagePicker;
import com.esafirm.imagepicker.model.Image;

import java.util.ArrayList;

public class RegisterUserActivity extends AppCompatActivity {

//TODO: Handle empty return from AddUserActivity

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_register_user);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.addUser);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                Intent intent = new Intent(RegisterUserActivity.this, AddUserActivity.class);
                RegisterUserActivity.this.startActivityForResult(intent, 10);

            }
        });
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        final Button button1 = (Button) findViewById(R.id.deleteButton);
        button1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //Creating the instance of PopupMenu
                PopupMenu popup = new PopupMenu(RegisterUserActivity.this, button1);
                //Inflating the Popup using xml file
                popup.getMenuInflater()
                        .inflate(R.menu.popup_menu, popup.getMenu());

                //registering popup with OnMenuItemClickListener



                popup.show(); //showing popup menu
            }
        }); //closing the setOnClickListener method
    }


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        //Intent intent = new Intent(RegisterUserActivity.this, AddUserActivity.class);
        if(data != null) {
            String userName = data.getStringExtra("UserName");
            Log.d("RegUser", userName);

            TextView tv = (TextView) findViewById(R.id.user_name);
            tv.setText(userName);
        }
    }

}
