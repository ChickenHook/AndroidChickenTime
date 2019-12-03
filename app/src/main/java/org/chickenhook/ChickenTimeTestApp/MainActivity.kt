package org.chickenhook.ChickenTimeTestApp

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import org.chickenhook.chickenTime.NativeInterface

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        NativeInterface.installHooks()
    }
}
