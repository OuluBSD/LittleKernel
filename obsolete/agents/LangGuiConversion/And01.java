
// 1. Hello World GUI (AndroidX)
// 2. Simple button with click -> Toast
// Note: These are Activity snippets for an Android app.
package com.example.langguiconversion;

import android.os.Bundle;
import android.util.TypedValue;
import android.view.Gravity;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class And01_HelloActivity extends AppCompatActivity {
    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setTitle("Hello World program");
        TextView tv = new TextView(this);
        tv.setText("Hello world!");
        tv.setGravity(Gravity.CENTER);
        setContentView(tv);
    }
}

class And01_ButtonActivity extends AppCompatActivity {
    private int dp(int v) {
        return (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, v, getResources().getDisplayMetrics());
    }

    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setTitle("Button program");

        FrameLayout root = new FrameLayout(this);
        Button btn = new Button(this);
        btn.setText("Hello world!");
        FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(dp(100), dp(30));
        lp.leftMargin = dp(30);
        lp.topMargin = dp(30);
        btn.setLayoutParams(lp);
        btn.setOnClickListener(v -> Toast.makeText(this, "Popup message", Toast.LENGTH_SHORT).show());
        root.addView(btn);
        setContentView(root);
    }
}
