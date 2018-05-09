# Live2D plugin for UE4

This is a WIP UE4 plugin for Live2D model.

## Usage

Use `CreateCubism` in `UCubismBpLib` to create the Live2D model viewer, you also need to manualy set `Model Texture` in Blueprint or C++ code. Then you can use `Get_RenderTarget()` to get the Live2d Model render target.

## Sample

As this is a wip plugin, it may not be so easy to make it work, you can use this [sample](https://www.dropbox.com/s/xznnetszfir6lng/Live2D.rar?dl=0) to see how it works.

## TODO

- Link Control API into plugin

- Make the asset editor usable

- Optimze the render efficient and the whole workflow