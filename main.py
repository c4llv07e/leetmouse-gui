#!/usr/bin/env python

import PySimpleGUIQt as sg
import os.path
from ModuleParameter import ModuleParameter

params = {
    "Acceleration": ModuleParameter("Acceleration"),
    "Sensitivity": ModuleParameter("Sensitivity"),
    "Speed Cap": ModuleParameter("SpeedCap"),
    "Sensitivity Cap": ModuleParameter("SensitivityCap"),
    "Offset": ModuleParameter("Offset"),
    "Scrolls per Tick": ModuleParameter("ScrollsPerTick"),
    "Midpoint": ModuleParameter("Midpoint"),
    "Exponent": ModuleParameter("Exponent")
}

modeLookup = {
    "Linear": 1,
    "Classic": 2,
    "Motivity": 3
}

def main():
    update = ModuleParameter("update")
    accelMode = ModuleParameter("AccelerationMode")
    accelModeVal = accelMode.parameterValue
    defaultAccelMode = "Linear"

    # get the acceleration mode to set the combo box
    for mode, key in modeLookup.items():
        if str(accelModeVal) == str(key):
            defaultAccelMode = mode

    layout = [[sg.Text("LEETMOUSE")]]
    layout.append([sg.Text("Mode: "),
                   sg.Combo(["Linear", "Classic", "Motivity"],
                            default_value=defaultAccelMode,
                            enable_events=True, key="modecombo")])

    for param in params:
        layout.append([sg.Text(param),
                       sg.InputText(default_text=params[param].parameterValue,
                                    key=params[param].parameterName)])

    layout.append([sg.Button("Update")])

    window = sg.Window(title="leetmouse GUI", layout=layout)

    while True:
        event, values = window.read()
        if event == "Update":
            # Update parameters
            for param in params:
                params[param].set(window[params[param].parameterName].get())
                accelMode.set(str(accelModeVal))

            # Set update flag so LEETMOUSE knows to read changes
            update.set("1")

        if event == "modecombo":
            # Mode was changed
            mode = values["modecombo"]
            # Save change but don't write it yet
            accelModeVal = modeLookup[mode]

        if event == sg.WIN_CLOSED:
            break

    window.close()


if __name__ == "__main__":
    main()
