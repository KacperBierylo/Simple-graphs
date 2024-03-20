#!/usr/bin/env python3


# Małe grafy proste reprezentowane jako para złożona ze zbioru wierzchołków i zbioru krawędzi.
class Graph:

    # Tworzy graf o podanej reprezentacji tekstowej (domyślnie: graf zerowy).
    def __init__( self, text = "?" ):
        self.__vertices, self.__edges, k, i = set( range( ord( text[0] ) - 63 ) ), set(), 0, 1
        for v in range( 1, len( self.__vertices ) ):
            for u in range( v ):
                if k == 0:
                    c, i, k = ord( text[i] ) - 63, i + 1, 6
                k -= 1
                if (c & (1 << k)) != 0:
                    self.__edges.add( (u, v) )

    # OPERACJA PODSTAWOWA
    # Zwraca liczbę wierzchołków grafu.
    def number_of_vertices( self ):
        return len( self.__vertices )

    # OPERACJA PODSTAWOWA
    # Zwraca zbiór wierzchołków grafu.
    def vertices( self ):
        return self.__vertices

    # OPERACJA PODSTAWOWA
    # Zwraca stopień wierzchołka.
    def vertex_degree( self, vertex ):
        return len( [u + v - vertex for u, v in self.__edges if u == vertex or v == vertex] )

    # OPERACJA PODSTAWOWA
    # Zwraca sąsiedztwo podanego wierzchołka.
    def vertex_neighbors( self, vertex ):
        return set( u + v - vertex for u, v in self.__edges if u == vertex or v == vertex )

    # OPERACJA PODSTAWOWA
    # Dodaje do grafu podany wierzchołek.
    def add_vertex( self, vertex ):
        self.__vertices.add( vertex )

    # OPERACJA PODSTAWOWA
    # Usuwa z grafu podany wierzchołek i wszystkie incydentne z nim krawędzie.
    def delete_vertex( self, vertex ):
        self.__vertices.discard( vertex )
        self.__edges -= set( (u, v) for u, v in self.__edges if u == vertex or v == vertex )

    # OPERACJA PODSTAWOWA
    # Zwraca liczbę krawędzi grafu.
    def number_of_edges( self ):
        return len( self.__edges )

    # OPERACJA PODSTAWOWA
    # Zwraca zbiór krawędzi grafu.
    def edges( self ):
        return self.__edges

    # OPERACJA PODSTAWOWA
    # Zwraca informację o tym, czy podane wierzchołki z sobą sąsiadują.
    def is_edge( self, u, v ):
        return (min( u, v ), max( u, v )) in self.__edges

    # OPERACJA PODSTAWOWA
    # Dodaje do grafu podaną krawędź.
    def add_edge( self, u, v ):
        self.__edges.add( (min( u, v ), max( u, v )) )

    # OPERACJA PODSTAWOWA
    # Usuwa z grafu podaną krawędź.
    def delete_edge( self, u, v ):
        self.__edges.discard( (min( u, v ), max( u, v )) )

    # Sprawdza, czy podane grafy są identyczne.
    def __eq__( self, graph ):
        return self.__vertices == graph.vertices() and self.__edges == graph.edges()

    # Sprawdza, czy podane grafy są różne.
    def __ne__( self, graph ):
        return self.__vertices != graph.vertices() or self.__edges != graph.edges()
    
    # OPERACJA DODATKOWA
    # Zwraca informację o tym, czy graf jest dwudzielny.
    def is_bipartite( self ):
        cs = {v: 0 for v in self.__vertices}
        #print(cs)
        def color_component( vertex, color ):
            cs[vertex] = color
            for v in self.__vertices:
                if self.is_edge( v, vertex ):
                    if cs[v] == color:
                        return False
                    if cs[v] == 0:
                        if not color_component( v, 3 - color ):
                            return False
            return True
        for v in self.__vertices:
            if cs[v] == 0:
                if not color_component( v, 1 ):
                    return False
        return True
