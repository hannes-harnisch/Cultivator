# Cultivator
Simulating cellular automata on the GPU with Vulkan, written from scratch, unless you count the Vulkan C++ bindings. This was done mostly as an exercise for learning the ins and outs of Vulkan.

The use of Vulkan makes the whole simulation very efficient. Running it at full speed causes the simulations to reach maximum entropy almost immediately. The images below were captured with 1-millisecond sleeps between draws.

The simulation is done via fragment shaders written in HLSL. At this point, there is no user interface, resolution scaling or runtime simulation selection and only Windows is supported. Camera-like functionality will be added in the future.

Currently comes with implementations of the Game of Life, Highlife, Day and Night, Seeds, Life Without Death and Brian's Brain automaton rules.

## Examples

<p>
  <img src="https://github.com/hannes-harnisch/hannes-harnisch/blob/main/CultivatorDayAndNight.gif" width="49%">
  <img src="https://github.com/hannes-harnisch/hannes-harnisch/blob/main/CultivatorGameOfLife.gif" width="49%">
  <img src="https://github.com/hannes-harnisch/hannes-harnisch/blob/main/CultivatorHighlife.gif" width="49%">
  <img src="https://github.com/hannes-harnisch/hannes-harnisch/blob/main/CultivatorLifeWithoutDeath.gif" width="49%">
</p>
