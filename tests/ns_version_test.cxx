#include <cstdio>

#include "ns/version.h"

int main() {
  printf("Project: %s\nVersion: %06X\nBranch: %s\nCommit Hash: %s\n",
      ns::kProjectName, ns::kVersion, ns::kBranchName, ns::kCommitHash);
  return 0;
}
