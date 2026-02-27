package com.derbmaroc.battle;
 
import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends Activity { 

    static {
        System.loadLibrary("HelloJni");
	}
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
		TextView tv = findViewById(R.id.activitymainTextView1);
		tv.setText(test());
    }
    
    public static native String test();

}