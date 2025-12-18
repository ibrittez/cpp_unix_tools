- acá a diferencia del básico agrego manejo de señales:

1. por facha.
2. porque si se cerraba el productor con ^C, no se cerraba la cola y quedaba el recurso
   tomado al pedo.

- además hice que MessageQueue sea una clase template, deje lo del ej anterior como
  especialización de string e implementé el template para plain old data.
