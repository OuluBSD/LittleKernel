
// 1. Hello World GUI (Babylon.js GUI 2D overlay)
// 2. Simple button with click -> message label
import { Engine, Scene } from '@babylonjs/core';
import { AdvancedDynamicTexture, Button, Control, TextBlock } from '@babylonjs/gui';

export function Bab01_Hello(engine: Engine, canvas: HTMLCanvasElement) {
  const scene = new Scene(engine);
  const gui = AdvancedDynamicTexture.CreateFullscreenUI('ui', true, scene);
  const label = new TextBlock('lbl', 'Hello world!');
  label.textHorizontalAlignment = Control.HORIZONTAL_ALIGNMENT_CENTER;
  label.textVerticalAlignment = Control.VERTICAL_ALIGNMENT_CENTER;
  gui.addControl(label);
  engine.runRenderLoop(() => scene.render());
  return scene;
}

export function Bab01_Button(engine: Engine, canvas: HTMLCanvasElement) {
  const scene = new Scene(engine);
  const gui = AdvancedDynamicTexture.CreateFullscreenUI('ui', true, scene);
  const btn = Button.CreateSimpleButton('btn', 'Hello world!');
  btn.width = '100px';
  btn.height = '30px';
  btn.horizontalAlignment = Control.HORIZONTAL_ALIGNMENT_LEFT;
  btn.verticalAlignment = Control.VERTICAL_ALIGNMENT_TOP;
  btn.left = '30px';
  btn.top = '30px';
  btn.onPointerUpObservable.add(() => {
    const msg = new TextBlock('msg', 'Popup message');
    msg.top = '70px';
    msg.left = '30px';
    msg.horizontalAlignment = Control.HORIZONTAL_ALIGNMENT_LEFT;
    msg.verticalAlignment = Control.VERTICAL_ALIGNMENT_TOP;
    gui.addControl(msg);
  });
  gui.addControl(btn);
  engine.runRenderLoop(() => scene.render());
  return scene;
}
