<?php
/**
 * Created by PhpStorm.
 * User: ronald
 * Date: 8/8/15
 * Time: 8:40 AM
 */

function bullpen_settings_form($form, &$form_state) {
  $form = array();

  $node_types = node_type_get_types();
  $allowed = array();
  foreach ($node_types as $key => $value ) {
    $allowed[$key] = $node_types[$key]->name;
  }
  $form['bullpen_supported_node_types'] = array(
    '#title' => t('Entity types'),
    '#type' => 'select',
    '#multiple' => TRUE,
    '#default_value' => variable_get('bullpen_supported_node_types', array()),
    '#required' => TRUE,
    '#options' => $allowed,
  );

  $current_path = variable_get('bullpen_path', '');

  $msg = '<p>' . t('The storage path to hold the files in the bullpen.') . ' ';
  $msg .= t('While files uploads are pending, they will be stored under the <em>private://</em> file uri scheme.') . ' ';
  $msg .= t('When files are approved, they are moved the the specified path for the content type.') . ' ';
  $msg .= '<strong>' . t('Do not add a trailing slash to the path.') . '</strong><br /><br />';
  $msg .= t('The current path: <em>@path</em>', array('@path' => ('private://' . $current_path) ));
  $msg .= '</p>';

  $form['bullpen_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path'),
    '#description' => $msg,
    '#default_value' => $current_path,
    '#size' => 30,
  );

  // We'll want to make sure some of the fields are in the format we expect so
  // this extra submit hanlder will allow us to do that.
  $form['#submit'][] = 'bullpen_settings_form_submit';

  //header('content-type:text/plain');
  //var_dump(system_settings_form($form));
  //exit;

  return system_settings_form($form);
}

function bullpen_settings_form_submit($form, &$form_state) {
  // Make sure that our path doesn't have a trailing slash.
  $path =& $form_state['values']['bullpen_path'];
  if (substr($path, strlen($path)-1, 1) == '/') {
    $path = substr($path, 0, strlen($path)-1);
    drupal_set_message(t('<em>Path</em> contained a trailing slash and it was removed.'), 'warning');
  }
  // Create the directory for file storage.
  $directory = 'private://' . $path;
  file_prepare_directory($directory, FILE_CREATE_DIRECTORY);
}