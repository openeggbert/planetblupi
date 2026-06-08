/**
 * @file fifo.h
 * @brief Priority-sorted stack (open-list) used by the A* pathfinder.
 *
 * CPileTriee is a min-heap ordered by distance that lets the pathfinding
 * algorithm in chemin.cpp efficiently retrieve the closest unvisited cell.
 */

#pragma once

// traitement d'une liste en fifo


// stucture pour enpiler des positions
// en fonction de leur distance ŕ la cible
typedef struct
{
	long		pos;
	long		dist;
} 
Element;


// traitement d'une pile triée

class CPileTriee
{
private:
	long		m_taille;		// nombre de polongs max
	long		m_max;			// position limite
	long		m_out;			// position pour reprendre
	Element*	m_data;			// données
			
public:
	CPileTriee(long taille);
	~CPileTriee();
	
	void		put(long pos, long dist);
	long		get();
};