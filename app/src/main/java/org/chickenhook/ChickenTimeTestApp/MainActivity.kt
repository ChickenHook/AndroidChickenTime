package org.chickenhook.ChickenTimeTestApp

import android.os.Bundle
import android.os.SystemClock
import android.util.Log
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import org.chickenhook.chickenTime.NativeInterface

class MainActivity : AppCompatActivity() {

    val callbackFunctionList = ArrayList<String>()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        findViewById<Button>(R.id.activtiy_main_do_reflection)?.let {
            it.setOnClickListener {
                MainActivity::class.java.getMethod("doIt")?.invoke(this)
                doIt2()
            }
        } ?: let {
            doIt();
        }

        findViewById<Button>(R.id.activtiy_main_install_hooks)?.apply {
            setOnClickListener{
                NativeInterface.installHooks()
            }
        }
        val text = findViewById<TextView>(R.id.activtiy_main_text);
        findViewById<Button>(R.id.activtiy_main_show_hook_callback)?.apply {
            setOnClickListener{
                text?.setText("" + callbackFunctionList.size)
            }
        }
        Thread {
            while (true) {
                SystemClock.sleep(1000)
                Log.d("MainActivity", "This background thread was triggered, yeah!")
                doIt()
                doIt2()
            }
        }.start();

        NativeInterface.setCallback { clazz, name ->
            // do something
            callbackFunctionList.add("$clazz.$name")
        }

    }

    fun doIt() {
        Log.d("MainActivity", "Yeah I was triggered!")
    }

    fun doIt2() {
        Log.d("MainActivity", "Do it 2 was triggered!")
    }

    override fun onResume() {
        super.onResume()

    }
}
