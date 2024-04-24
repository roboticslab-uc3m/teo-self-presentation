# TEO self presentation

Demonstration of the humanoid robot TEO presenting itself ([video](https://www.youtube.com/watch?v=VSNGP9K1_YQ)).

[![IMAGE ALT TEXT](http://i3.ytimg.com/vi/VSNGP9K1_YQ/maxresdefault.jpg)](https://www.youtube.com/watch?v=VSNGP9K1_YQ "Teo Self Presentation")

## Installation

Installation instructions for installing from source can be found [here](doc/teo-self-presentation-install.md).

## Usage

Refer to the [demo procedure guide](https://robots.uc3m.es/teo-developer-manual/demo-procedure.html) for the initial robot setup steps.

1. In manipulation PC run `launchCanBus --from manipulation-noHead-demo.ini`.
2. In head PC run `speechSynthesis --backend piper --model es_ES-davefx-medium --prefix /tts`. If you want to use the english voice use `en_US-lessac-medium` instead.
3. In your local PC run `bodyExecution --robot /teo`.
4. In your local PC run `dialogueManager --language spanish --backend piper`.
5. In your local PC run `yarpmanager`, in the yarpmanager GUI go to `Apllication/teo-self-presentation_spanish_App` and click `Connect All` button to start the presentation.

Once installed, run `yarpmanager` and look for the language version of the launcher XML file you are most interested in (teo-self-presentation_*language*.xml). If not present, you can manually add this application by means of the "Open File" or "Import Folder" buttons.

Double click on the app and press "Run all", await until all modules have successfully loaded. Then, press "Connect all". The robot should start moving and speaking.

## Contributing

#### Posting Issues

1. Read [CONTRIBUTING.md](CONTRIBUTING.md)
2. [Post an issue / Feature request / Specific documentation request](https://github.com/roboticslab-uc3m/teo-self-presentation/issues)

#### Fork & Pull Request

1. [Fork the repository](https://github.com/roboticslab-uc3m/teo-self-presentation/fork)
2. Create your feature branch (`git checkout -b my-new-feature`) off the `master` branch, following the [Forking Git workflow](https://www.atlassian.com/git/tutorials/comparing-workflows/forking-workflow)
3. Commit your changes
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request

## Status

[![CI (Linux)](https://github.com/roboticslab-uc3m/teo-self-presentation/workflows/Continuous%20Integration/badge.svg)](https://github.com/roboticslab-uc3m/teo-self-presentation/actions)

[![Issues](https://img.shields.io/github/issues/roboticslab-uc3m/teo-self-presentation.svg?label=Issues)](https://github.com/roboticslab-uc3m/teo-self-presentation/issues)
