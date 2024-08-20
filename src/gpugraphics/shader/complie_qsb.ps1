Set-Location $PSScriptRoot

$qsb = "C:\Qt\6.7.2\msvc2019_64\bin\qsb.exe"

& $qsb --glsl "150,120,100 es" --hlsl 50 --msl 12 -o vulkan.vert.qsb vulkan.vert
& $qsb --glsl "150,120,100 es" --hlsl 50 --msl 12 -o vulkan.frag.qsb vulkan.frag
