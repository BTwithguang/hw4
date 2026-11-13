#ifndef RECCHECK
#include <cstdlib>
#endif

#include "equal-paths.h"

namespace {

// Returns the subtree's leaf depth.  -1 means that unequal leaf depths
// were found somewhere below this node.
int equalHeight(Node* root)
{
    if(root == nullptr) {
        return 0;
    }

    if(root->left == nullptr && root->right == nullptr) {
        return 1;
    }

    if(root->left == nullptr) {
        int rightHeight = equalHeight(root->right);
        return rightHeight == -1 ? -1 : rightHeight + 1;
    }

    if(root->right == nullptr) {
        int leftHeight = equalHeight(root->left);
        return leftHeight == -1 ? -1 : leftHeight + 1;
    }

    int leftHeight = equalHeight(root->left);
    int rightHeight = equalHeight(root->right);

    if(leftHeight == -1 || rightHeight == -1 || leftHeight != rightHeight) {
        return -1;
    }

    return leftHeight + 1;
}

} // namespace

bool equalPaths(Node* root)
{
    return equalHeight(root) != -1;
}
