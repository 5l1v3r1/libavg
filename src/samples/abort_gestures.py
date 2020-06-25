#!/usr/bin/env python
# -*- coding: utf-8 -*-

from libavg import avg, gesture, app

import gestures

RESOLUTION = avg.Point2D(800, 600)

nodeList = []
nodesEnabled = True

def abortAll():
    for node in nodeList:
        node.recognizer.abort()

def switchNodesEnabled():
    global nodesEnabled
    nodesEnabled = not nodesEnabled
    for node in nodeList:
        node.recognizer.enable(nodesEnabled)


class TapButton(gestures.TextRect):
    def __init__(self, text, **kwargs):
        super().__init__(text, **kwargs)

        self.recognizer = gesture.TapRecognizer(node=self,
                    possibleHandler=self._onPossible, detectedHandler=self._onDetected,
                    failHandler=self._onFail)

    def _onPossible(self):
        self.rect.fillcolor = "FFFFFF"

    def _onDetected(self):
        self.rect.fillcolor = "000000"
        self.rect.color = "00FF00"

    def _onFail(self):
        self.rect.fillcolor = "000000"
        self.rect.color = "FF0000"


class AbortButton(TapButton):
    def __init__(self, text, **kwargs):
        super().__init__(text, **kwargs)

    def _onPossible(self):
        super()._onPossible()
        self.words.color = "000000"

    def _onDetected(self):
        super()._onDetected()
        abortAll()
        self.words.color = "FFFFFF"

    def _onFail(self):
        super()._onFail()
        self.words.color = "FFFFFF"


class EnableButton(TapButton):
    def __init__(self, text, **kwargs):
        super().__init__(text, **kwargs)

        self.words.color = "FF0000"

    def changeText(self):
        if(nodesEnabled):
            self.words.text = "Disable all"
            self.words.color = "FF0000"
        else:
            self.words.text = "Enable all"
            self.words.color = "00FF00"

    def _onDetected(self):
        super()._onDetected()
        switchNodesEnabled()
        self.changeText()


class GestureDemoDiv(app.MainDiv):

    def onInit(self):

        avg.WordsNode(text='''a - abort recognition <br/>
                d - enable/disable recognition <br/><br/>
                or use the buttons on the right side''',
                pos=(20, 510), parent=self)

        nodeList.append(gestures.HoldNode(text="HoldRecognizer", pos=(20,20),
                parent=self))

        nodeList.append(gestures.DragNode(text="DragRecognizer<br/>friction",
                pos=(200,20), friction=0.05, parent=self))

        nodeList.append(gestures.TransformNode(text="TransformRecognizer",
                ignoreRotation=False, ignoreScale=False, pos=(380,20), parent=self))

        self.abortButton = AbortButton(text="Abort all", pos = (630, 490), parent=self)

        self.enableButton = EnableButton(text="Disable all", pos = (630, 540),
                parent=self)

        app.keyboardmanager.bindKeyDown(text="a", handler=abortAll,
                help="abort recognition")
        app.keyboardmanager.bindKeyDown(text="d", handler=self.onEnableKey,
                help="Enable/disable recognition")

    def onEnableKey(self):
        switchNodesEnabled()
        self.enableButton.changeText()


if __name__ == '__main__':
    app.App().run(GestureDemoDiv(), app_resolution="800,600")
