#include <cstdio>

#include "ns/version.hpp"
#include "ns/vcs/metadata.hpp"

int main() {
  printf("Project: %s\n", ns::kProjectName);
  printf("Version: %06X\n", ns::kVersion);
  printf("Branch: %s\n", ns::vcs::kBranchName);
  printf("Commit Hash: %s\n", ns::vcs::kCommitHash);
  printf("Local Changes: %d\n", ns::vcs::kHasLocalChanges);
  return 0;
}
