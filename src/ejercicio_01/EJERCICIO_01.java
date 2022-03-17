package ejercicio_01;

import Entidades.Libro;
import Servicio.Servicio;

public class EJERCICIO_01 
{
    public static void main(String[] args)
    {
        Servicio ls = new Servicio();
        Libro l1 = ls.cargarLibro();
        ls.mostrarLibro(l1);
    }
    
}
