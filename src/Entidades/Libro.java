
package Entidades;

public class Libro 
{
    private int isbn;
    private String titulo;
    private String autor;
    private int numpaginas; 
    
    //Constructor por defecto

    public Libro() {
    }
    
    //Constructor por parametros

    public Libro(int isbn, String titulo, String autor, int numpaginas) {
        this.isbn = isbn;
        this.titulo = titulo;
        this.autor = autor;
        this.numpaginas = numpaginas;
    }
    
    //getter & setter

    public int getIsbn() {
        return isbn;
    }

    public void setIsbn(int isbn) {
        this.isbn = isbn;
    }

    public String getTitulo() {
        return titulo;
    }

    public void setTitulo(String titulo) {
        this.titulo = titulo;
    }

    public String getAutor() {
        return autor;
    }

    public void setAutor(String autor) {
        this.autor = autor;
    }

    public int getNumpaginas() {
        return numpaginas;
    }

    public void setNumpaginas(int numpaginas) {
        this.numpaginas = numpaginas;
    }      
    
    //metodo toString

    @Override
    public String toString() {
        return "Libro{" + "isbn=" + isbn + ", titulo=" + titulo + ", autor=" + autor + ", numpaginas=" + numpaginas + '}';
    }
    
}
