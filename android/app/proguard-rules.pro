# Add project specific ProGuard rules here.
# SDL3 loads native code via JNI; keep all SDL classes.
-keep class org.libsdl.app.** { *; }
-keep class org.openeggbert.planetblupi.** { *; }
