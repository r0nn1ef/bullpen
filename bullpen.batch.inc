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

function bullpen_process_approved_file($file, $bullpen, $node) {
  // Add the $file->fid to the node's image field.

  // Save the node.

  // Add the file to the file_usage table for the node.

  // Remove the $file->fid from the bullpen request.
}

function bullpen_process_denied_file($file, $bullpen) {
  // Delete the physical file from the system.

  // Remove the file reference in the bullpen.
}

function bullpen_batch_cleanup($bullpen) {
  // Query to get number of files remaining (should be 0)

  // If above query returns 0, delete the bullpen item.

  // Otherwise, set a message about the problem.
}

function bullpen_batch_finished() {

}