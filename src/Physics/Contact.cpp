#include "Contact.h"

#include "Body.h"

/*// move the bodies apart with projection method
float totalInverseMass = bodies[i]->inverseMass + bodies[j]->inverseMass;
bodies[i]->position -= contact.normal * (contact.depth * bodies[i]->inverseMass / totalInverseMass);
bodies[j]->position += contact.normal * (contact.depth * bodies[j]->inverseMass / totalInverseMass);
 */

void Contact::ResolvePenetration() const
{
    if (a->IsStatic() && b->IsStatic()) return;
    
    float da = depth / (a->inverseMass + b->inverseMass) * a->inverseMass;
    float db = depth / (a->inverseMass + b->inverseMass) * b->inverseMass;

    a->position -= normal * da;
    b->position += normal * db;
}