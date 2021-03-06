/* Code By Freddy Demiane */
#include "Octree.h"
#include <math.h>
#include <vector>
#include <pthread.h>
#include "Pack.h"
#define NODESIZE 100

Octree::Octree()
{

}
int getDepth(int threadCount)
{
  int d = 0;
  int n = 1;
  while(threadCount>n)
    {
      d++;
      n*=8;
    }
  return d;
  
}

void* wrapper2(void* morton)
{
  Pack* obj = (Pack*) morton;
  
  obj->obj->buildOctreeAssign(obj->morton);
  
}
int getPower(int depth, int numberOfThreads)
{
  if(numberOfThreads > pow(8 , depth))
    return getPower(depth+1 , numberOfThreads);
  return pow(8 , depth);
}
vector<int> buildCurve(int mortonCode)
{
  vector<int> curve;
  while(mortonCode>1)
    {
      int rem = mortonCode%8;
      cout<<"rem is "<<rem<<" "<<mortonCode<<endl;
      curve.push_back(rem);
      mortonCode /=8;
    }
  return curve;
}
void Octree::assignJobs(int numberOfThreads)
{
  int power = getPower(0 , numberOfThreads);
  for(int i = 0 ; i<power ; i++) 
    mortonCodes[i%numberOfThreads].push_back(power+i);
   
}
void Octree::init(int numberOfThreads)
{
  tCount = numberOfThreads;
  cout<<"d15 :"<< getDepth(15)<<endl;
  cout<<"d1 : "<<getDepth(1)<<endl;
  threads = new pthread_t[numberOfThreads];
  mortonCodes = new vector<int>[numberOfThreads];
  ZCurves = new vector<int>[numberOfThreads];
  pack = new Pack[numberOfThreads];  
  assignJobs(numberOfThreads);
  for(int i = 0 ; i < numberOfThreads ; i++)
    for(int j = 0 ; j <mortonCodes[i].size() ; j++)
      cout<<mortonCodes[i][j]<<" is the morton code of thread: "<<i<<endl;
  
}
void Octree::buildOctreeAssign(vector<int>* mortonCode)
{
  vector<int> mortonCodes = *mortonCode;
  
    for(int j = 0 ; j<mortonCodes.size() ; j++){
         vector<int> seq = buildCurve(mortonCodes[j]);
         Node* temp = root;
         for(int c = seq.size()-1 ; c>=0 ; c--){
	   //	   cout<<seq[c]<<" is the seq\n";
	   if(temp == NULL)
	     cout<<"DEATH"<<endl;
	   temp = root->nodes[seq[c]];
	 }
	 
       	  buildOctreeParallel(temp);
    }
  
    
  
}
void Octree::buildOctreeParallel(Node* leaf)
{
  if(leaf==NULL)
    {
      cout<<"ADFDASAFdasfads"<<endl;
      return;
    }
    vec3 center(0, 0, 0);
  for (int i = 0; i < leaf->particles.size(); i++) {
    center += leaf->particles[i]->position;
  }
  center /= leaf->particles.size();
  leaf->center = center;

  for (int i = 0; i < 8; i++)
    leaf->nodes[i] = new Node();
 
  
     for (int i = 0; i < leaf->particles.size(); i++)
    {
      //cout << "2 \n";
      vec3 position = leaf->particles[i]->position;
       if (position.x <= center.x && position.y <= center.y && position.z <= center.z)
	{
	  
	  leaf->nodes[0]->particles.push_back(leaf->particles[i]);
	  //cout << leaf->nodes[0]->particles[0]->mass<<" ";
	}
      else if (position.x <= center.x && position.y <= center.y && position.z >= center.z)
	{

	  leaf->nodes[1]->particles.push_back(leaf->particles[i]);
	}
      else if (position.x <= center.x && position.y >= center.y && position.z <= center.z)
	{

	  leaf->nodes[2]->particles.push_back(leaf->particles[i]);
	}
      else if (position.x <= center.x && position.y >= center.y && position.z >= center.z)
	{

	  leaf->nodes[3]->particles.push_back(leaf->particles[i]);
	}
      else if (position.x >= center.x && position.y <= center.y && position.z <= center.z)
	{

	  leaf->nodes[4]->particles.push_back(leaf->particles[i]);
	}
      else if (position.x >= center.x && position.y <= center.y && position.z >= center.z)
	{

	  leaf->nodes[5]->particles.push_back(leaf->particles[i]);
	}
      else if (position.x >= center.x && position.y >= center.y && position.z <= center.z)
	{

	  leaf->nodes[6]->particles.push_back(leaf->particles[i]);
	}
      else if (position.x >= center.x && position.y >= center.y && position.z >= center.z)
	{

	  leaf->nodes[7]->particles.push_back(leaf->particles[i]);
	  }

    }
   
  for (int i = 0; i < 8; i++)
    {
      if(leaf->nodes[i]->particles.size()>NODESIZE)
	buildOctreeParallel(leaf->nodes[i]);
  	}
   
  
}
void Octree::buildOctree(std::vector<Particle>& arr,int threadCount)
{
  cout << "Building...\n";
  root = new Node();
  for (int i = 0; i < arr.size(); i++) {

    root->particles.push_back(&arr[i]);
  }

  if (root->particles.size() > NODESIZE) {
    buildOctree(root, 1, threadCount);
  }



}

void Octree::buildOctree(Node* leaf, int depth, int threadCount)
{
  int reqDepth = getDepth(threadCount);
  
  vec3 center(0, 0, 0);
  for (int i = 0; i < leaf->particles.size(); i++) {
    center += leaf->particles[i]->position;
    //cout << "pos " << leaf->particles[i]->position.x<<endl;
  }
  center /= leaf->particles.size();
  leaf->center = center;
  //cout << "Center is: " << center.x<<" "<<center.y<<" "<<center.z<<endl;

  for (int i = 0; i < 8; i++)
    leaf->nodes[i] = new Node();
  //cout << "1 \n";

  for (int i = 0; i < leaf->particles.size(); i++)
    {
      //cout << "2 \n";
      vec3 position = leaf->particles[i]->position;
      if (position.x <= center.x && position.y <= center.y && position.z <= center.z)
	{//cout <<"i : "<<i <<" "<< leaf->particles[i]->position.x << " " << position.y << " " << position.z << endl;
	  if (center.x == 0) {

	    //cout << " 1 " << endl;
	    //continue;

	  }
	  leaf->nodes[0]->particles.push_back(leaf->particles[i]);
	  //cout << leaf->nodes[0]->particles[0]->mass<<" ";
	}
      else if (position.x <= center.x && position.y <= center.y && position.z >= center.z)
	{

	  leaf->nodes[1]->particles.push_back(leaf->particles[i]);
	}
      else if (position.x <= center.x && position.y >= center.y && position.z <= center.z)
	{

	  leaf->nodes[2]->particles.push_back(leaf->particles[i]);
	}
      else if (position.x <= center.x && position.y >= center.y && position.z >= center.z)
	{

	  leaf->nodes[3]->particles.push_back(leaf->particles[i]);
	}
      else if (position.x >= center.x && position.y <= center.y && position.z <= center.z)
	{

	  leaf->nodes[4]->particles.push_back(leaf->particles[i]);
	}
      else if (position.x >= center.x && position.y <= center.y && position.z >= center.z)
	{

	  leaf->nodes[5]->particles.push_back(leaf->particles[i]);
	}
      else if (position.x >= center.x && position.y >= center.y && position.z <= center.z)
	{

	  leaf->nodes[6]->particles.push_back(leaf->particles[i]);
	}
      else if (position.x >= center.x && position.y >= center.y && position.z >= center.z)
	{

	  leaf->nodes[7]->particles.push_back(leaf->particles[i]);
	}

    }
    if(depth >=reqDepth)
    {
      for(int i = 0 ; i < threadCount ; i++)
	{
	  //	  morton = mortonCodes[i];
	  
	  pack[i].morton = &mortonCodes[i];
	  for(int j = 0 ; j <mortonCodes[i].size(); j++)
	    cout<<"morton: "<<mortonCodes[i][j]<<" ";
	  cout<<endl;
	  pack[i].obj = this;

	  //      buildOctreeAssign(&morton);
	  pthread_create(&threads[i], NULL ,wrapper2 ,&pack[i]);
	}
      for(int i = 0 ; i<threadCount ; i++)
	pthread_join(threads[i], NULL);

      return;
      }
  for (int i = 0; i < 8; i++)
    {
      if(leaf->nodes[i]->particles.size()>NODESIZE)
	buildOctree(leaf->nodes[i], depth+1 , threadCount);
    }



}

std::vector<Particle*> Octree::searchOctree(Node* leaf, Particle& particle)
{

  if (leaf->particles.size() <= NODESIZE||leaf->nodes[0] == NULL ) {
    
    //cout << leaf->particles[0]->mass;
    return leaf->particles;
  }


  //cout << "Test" << endl;
  vec3 position = particle.position;
  vec3 center = leaf->center;
  if (position.x <= center.x && position.y <= center.y && position.z <= center.z)
    {
      //      cout<<"Hi";
      return searchOctree(leaf->nodes[0], particle);
    }
  else if (position.x <= center.x && position.y <= center.y && position.z >= center.z)
    {
      return searchOctree(leaf->nodes[1], particle);
    }
  else if (position.x <= center.x && position.y >= center.y && position.z <= center.z)
    {
      return searchOctree(leaf->nodes[2], particle);
    }
  else if (position.x <= center.x && position.y >= center.y && position.z >= center.z)
    {
      return searchOctree(leaf->nodes[3], particle);
    }
  else if (position.x >= center.x && position.y <= center.y && position.z <= center.z)
    {
      return searchOctree(leaf->nodes[4], particle);
    }
  else if (position.x >= center.x && position.y <= center.y && position.z >= center.z)
    {
      return searchOctree(leaf->nodes[5], particle);
    }
  else if (position.x >= center.x && position.y >= center.y && position.z <= center.z)
    {
      return searchOctree(leaf->nodes[6], particle);
    }
  else if (position.x >= center.x && position.y >= center.y && position.z >= center.z)
    {
      
      return searchOctree(leaf->nodes[7], particle);
    }


  return leaf->particles;

}

std::vector<Particle*> Octree::searchOctree(Particle& particle)
{
  //cout << "Searching...\n";
  //return root->particles;
  return searchOctree(root, particle);
}
void Octree::Clean(Node* leaf)
{
  
  if (leaf == NULL)
    return;
  for (int i = 0; i < 8; i++) {
    Clean(leaf->nodes[i]);
    delete leaf->nodes[i];
  }
}

void Octree::Clean(int threadCount)
{
  for(int i = 0 ; i < threadCount ; i++)
    mortonCodes[i].clear();
  Clean(root);
  delete root;
}
