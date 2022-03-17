
package Servicio;

import Entidades.Libro;
import java.util.Scanner;

public class Servicio 
{
    public Libro cargarLibro()
    {
        Libro l1 = new Libro();
        
        Scanner leer = new Scanner(System.in).useDelimiter("\n");
        
        System.out.println("Ingrese el numero de ISBN: ");
        l1.setIsbn(leer.nextInt());
        
        System.out.println("Ingrese Titulo del Libro: ");
        l1.setTitulo(leer.next());
        
        System.out.println("Ingrese Nombre de Autor: ");
        l1.setAutor(leer.next());
        
        System.out.println("Ingrese Número de paginas: ");
        l1.setNumpaginas(leer.nextInt());
        
        return l1;
        
    }
    
    public void mostrarLibro(Libro l1)
    {        
        Scanner leer = new Scanner(System.in).useDelimiter("\n");
        
        System.out.println("El numero de ISBN es: " + l1.getIsbn());       
       
        System.out.println("El Titulo del Libro es: " + l1.getTitulo());        
        
        System.out.println("El Nombre de Autor es: " + l1.getAutor());
             
        System.out.println("El Número de paginas es: " + l1.getNumpaginas());
        
        
    }
}
