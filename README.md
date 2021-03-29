Simple shell utility for reading wikipedia articles.

Requires:

- libcurl
- CJSON: https://github.com/DaveGamble/cJSON
- CUnit


Working:
  - Basic Requests for articles without ambiguity
  - Handling disambiguation pages


TODO: 
  - Handle articles that do NOT redirect to disambiguation page, but have
    multiple interpretations

  - Implement testing (cunit?)

  - Refactor and improve codebase
