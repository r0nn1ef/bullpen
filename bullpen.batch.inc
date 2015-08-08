<?php
/**
 * @file bullpen.batch.inc
 *
 * Batch API functions that will process files submitted by users to add to
 * an entity (node).
 *
 * Created by PhpStorm.
 * User: ronald
 * Date: 8/7/15
 * Time: 7:49 AM
 */

function bullpen_process_approved_file($files, $bullpen, $node, &$context) {
  // @todo this needs to be dynamic, but for now set the name of the field manually.
  $field_name = 'field_image';
  // Get the directory where the files for the node type are stored.
  $image_field = field_info_instance('node', $field_name, $node->type);
  $field_info = field_info_field($field_name);
  $path = $field_info['settings']['uri_scheme'] . '://' . $image_field['settings']['file_directory'];
  $sources = array();

  // Move the bullpen files to the new directory.
  foreach ($files as $file) {
    $source = $file;
    $destination = $path . (substr($path, (strlen($path)-1), 1) != '/' ? '/' : '') . $source->filename;
    $file = file_move($source, $destination);
    // Add the $file->fid to the node's image field and save the node.
    $node->field_image[LANGUAGE_NONE][] = (array)$file;
    // Add the fid to an array so we can remove it from the bullpen files table.
    $sources[] = $source->fid;
  }

  // Save the node with the new files attached.
  node_save($node);

  // Remove the $file->fid from the bullpen request.
  db_delete('bullpen_files')
    ->condition('fid', $sources, 'IN')
    ->condition('rid', $bullpen->rid, '=')
    ->execute();
}

function bullpen_process_denied_file($files, $bullpen, &$context) {
  bullpen_delete_files($files);
}

function bullpen_batch_cleanup($bullpen, &$context) {
  bullpen_remove_request($bullpen->rid);
}

function bullpen_batch_finished() {

}