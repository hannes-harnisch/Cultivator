# Cultivator
Simulating cellular automata on the GPU with Vulkan, written from scratch, unless you count the C++ standard library. This was done mostly as an exercise for learning the ins and outs of Vulkan.

The use of Vulkan makes the whole simulation very efficient. Running it at full speed causes the simulations to reach maximum entropy almost instantly. The images below were captured with 1-millisecond sleeps between draws.

The simulation is done via fragment shaders written in HLSL. At this point, there is no user interface, resolution scaling or runtime simulation selection and only Windows is supported. Camera-like functionality will be added in the future.

Currently comes with implementations of the Game of Life, Highlife, Day and Night, Seeds, Life Without Death and Brian's Brain automaton rules.

## Examples

<p>
  <a href="https://github.com/hannes-harnisch/hannes-harnisch/blob/main/Originals/CultivatorDayAndNight.gif"><img src="https://github.com/hannes-harnisch/hannes-harnisch/blob/main/CultivatorDayAndNight.gif" width="49%"></a>
  <a href="https://github.com/hannes-harnisch/hannes-harnisch/blob/main/Originals/CultivatorGameOfLife.gif"><img src="https://github.com/hannes-harnisch/hannes-harnisch/blob/main/CultivatorGameOfLife.gif" width="49%"></a>
  <a href="https://github.com/hannes-harnisch/hannes-harnisch/blob/main/Originals/CultivatorHighlife.gif"><img src="https://github.com/hannes-harnisch/hannes-harnisch/blob/main/CultivatorHighlife.gif" width="49%"></a>
  <a href="https://github.com/hannes-harnisch/hannes-harnisch/blob/main/Originals/CultivatorLifeWithoutDeath.gif"><img src="https://github.com/hannes-harnisch/hannes-harnisch/blob/main/CultivatorLifeWithoutDeath.gif" width="49%"></a>
</p>
