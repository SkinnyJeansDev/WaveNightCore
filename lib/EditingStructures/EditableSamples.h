typedef struct EditableSamples
{
  int NumNodes;
  Node *Root;
  
} EditableSamples;

typedef struct Node
{
  Node* Next;
  Node* Prev;
  void* Value; // A pointer to some value likely in my case to a sample :)
};

void append(Node* node,EditableSamples* samples)
{
  // If there is no root
  if(!samples->Root)
  {
    samples->Root = node;
  }
  else
  {
    Node* Runner = samples->Root;
    while (Runner->Next == 0) // While not null traverse the linked list
    {
      Runner = Runner->Next;
    }
    Runner->Next = node;
    node->Prev = Runner;
  }
  samples->NumNodes++;
}

void addBehind(Node* node,Node* existingNode){
  Node* previousNextNode = existingNode->Next; // Save the current next node before we replace it with the new node
  
  node->Next = previousNextNode; // Set the new node's next node to the one it replaced
  node->Prev = existingNode;

  existingNode->Next = node; // Complete the replacement
}

void addFront(Node* node, Node* existingNode){
  Node* existingPrevNode = existingNode->Prev;   

  node->Next = existingNode; 
  node->Prev = existingPrevNode;

  existingNode->Prev = node; // Complete the replacement
}

void remove(Node* node,EditableSamples* samples)
{
  if(!samples->Root)
  {
    samples->Root = node;
  }
  else
  {
    Node* Runner = samples->Root;
    while (Runner->Next != node) // While not null traverse the linked list
    {
      Runner = Runner->Next;
    }
    Runner->Next = node;
    
  }
}