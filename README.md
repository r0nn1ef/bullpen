File Bullpen
=============

CONTENTS OF THIS FILE
---------------------
   
 * Introduction
 * Requirements
 * Installation
 * Configuration
 * Maintainers
 
 Introduction
 ------------
 
 The File Bullpen module was specifically developed to allow users whom have the 'upload bullpen files' permission to upload files to photo galleries for approval. All files are uploaded to the <em>private://</em> file scheme in the Drupal installation.
 
 Being developed for a specific use case for a specific client, it is unlikely this module will be useful to anyone other than the client unless modifications are made. For the most part, this module is dynamic as possible when attaching files to nodes, but the file/image fields are hard coded to known fields used on the client website.
 
 Requirements
 ------------
 
 <strong>Required modules</strong>
 
 * [Private files download permission module](http://drupal.org/project/private_files_download_permission) &ndash; this is required to override the private file schema permissions for administrative roles
 * [Plupload integration module](http://drupal.org/project/plupload) &ndash; allows multiple file uploads
 * File (core module)
 
 <strong>Required Libraries</strong>
 
 * [Plupload library](http://www.plupload.com) (tested with version 1.5.8)
 
 
 Installation
 ------------
 
 To install the File Bullpen module, follow the module installation instructions for the required projects and libraries listed above. General module installation instructions are found at [https://www.drupal.org/documentation/install/modules-themes/modules-7](https://www.drupal.org/documentation/install/modules-themes/modules-7).
 
 Configuration
 -------------
 
 The Bullpen Files module has a administrative configuration form which allows the selection of the content type that will be allowed to have files attached for approval as well as setting the path under the <em>private://</em> file scheme where the pending files are stored. To view this configuration form, go to Administration &#187; Configuration &#187; Media &#187; Bullpen.
 
 Maintainers
 -----------
 This module is maintained by Ron Ferguson (<r0nn1ef8085@gmail.com>) for the [Scubadillos Dive Club](http://www.scubadillos.org). There is currently no other support for this module and features/bug fixes will only be added at the original clients request.