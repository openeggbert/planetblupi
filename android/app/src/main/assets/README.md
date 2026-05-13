# Planet Blupi Android Assets

This directory must contain the game asset directories before building the APK.

Planet Blupi expects the following layout at runtime:

```
assets/
  data/         <- from <planetblupi>/data/
  image/        <- from <planetblupi>/image/
  sound/        <- from <planetblupi>/sound/english/  (or your preferred locale)
  movie/        <- from <planetblupi>/movie/
```

## Populating assets

Copy or symlink the required directories from the project root:

```bash
cd android/app/src/main/assets

# Linux / macOS — symbolic links (recommended, saves disk space)
ln -s ../../../../../data  data
ln -s ../../../../../image image
ln -s ../../../../../sound/english sound
ln -s ../../../../../movie movie

# Or copy them (required on some platforms / CI)
cp -r ../../../../../data  data
cp -r ../../../../../image image
cp -r ../../../../../sound/english sound
cp -r ../../../../../movie movie
```

> **Note:** Android's asset packager follows symbolic links.
> The APK asset merger will include all files found under each linked directory.
> Other sound locales (deutsch, francais, us) can replace the `sound` link/copy
> if you want to ship a different language.

See `ANDROID.md` in the project root for full build instructions.
