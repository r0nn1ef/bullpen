<?php
/**
 * Created by PhpStorm.
 * User: rrf011
 * Date: 8/4/2015
 * Time: 9:58 AM
 */

function bullpen_upload_form($form, &$form_state) {
  $form = array();

  return $form;
}

function bullpen_upload_form_submit($form, &$form_state) {

}

function bullpen_upload_form_validate($form, &$form_state) {

}

function bullpen_approval_form($form, &$form_state, $node) {
  $form = array();

  // Capture our node as a property so we can use it in the submit handler.
  $form['#node'] = $node;

  return $form;
}

function bullpen_approval_form_submit($form, &$form_state) {

}

function bullpen_approval_form_validate($form, &$form_state) {

}

function bullpen_item_approval_form($form, $form_state, $bullpen_item) {

}

function bullpen_item_approval_form_submit($form, $form_state) {

}

/**
 * Menu callback to get published node titles for files to be attached to. Adapted
 * from the @see user_autocomplete() in the user module.
 * @param string $name
 */
function bullpen_autocomplete($string='') {
  // @todo load allowed types from settings.
  $types = array();
  $matches = array();
  if ($string) {
    $query = db_select('node', 'n')
      ->fields('n', array('title'))
      ->condition('n.title', db_like($string) . '%', 'LIKE')
      ->condition('n.status', 1, '=');

    if ( count($types) ) {
      $query->condition('n.type', $types, 'IN');
    }

    $result = $query->range(0, 10)->execute();

    foreach ($result as $node) {
      $matches[$node->title] = check_plain($node->title);
    }
  }

  drupal_json_output($matches);
}