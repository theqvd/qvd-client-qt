# Translations

These are the translation files for the QVD client.

## Adding a new translation

1. Find the code for your language, eg `es_MX`
2. Copy an existing file: `cp QVD_Client_es_ES.ts QVD_Client_es_MX.ts`
3. Edit the translation: `linguist-qt5 QVD_Client_es_MX.ts`
4. Go to *"Edit/Translation File Settings"* and adjust the target language.
5. Translate.

The new locale file will be automatically picked up by the build process.

## Maintenance

Run `./update_translations.sh` to scan the source code for new changes, and add new strings to the .ts files. After that, run *Qt Linguist* to add translations for the new strings.

## Installation

The client looks for translation files in the paths `i18n` (same directory as the binary), `/usr/share/QVD_Client/i18n` and `/usr/lib/qvd/share/QVD_Client/i18n`. This is defined in a constant on top of `main.cpp`.

