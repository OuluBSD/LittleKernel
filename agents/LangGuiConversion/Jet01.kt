
// 1. Hello World GUI (Jetpack Compose)
// 2. Simple button with click -> Toast
// Note: ComposeActivity snippet for Android.
package com.example.langguiconversion

import android.os.Bundle
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

class Jet01_HelloActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        title = "Hello World program"
        setContent { HelloWorldScreen() }
    }
}

class Jet01_ButtonActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        title = "Button program"
        setContent { ButtonScreen() }
    }
}

@Composable
fun HelloWorldScreen() {
    Box(Modifier.fillMaxSize()) {
        Text("Hello world!", modifier = Modifier.align(Alignment.Center))
    }
}

@Composable
fun ButtonScreen() {
    val ctx = LocalContext.current
    Box(Modifier.fillMaxSize()) {
        Button(
            onClick = { Toast.makeText(ctx, "Popup message", Toast.LENGTH_SHORT).show() }
        , modifier = Modifier
                .align(Alignment.TopStart)
                .padding(start = 30.dp, top = 30.dp)
                .size(width = 100.dp, height = 30.dp)
        ) {
            Text("Hello world!")
        }
    }
}

// Uses Toast for the popup message.
