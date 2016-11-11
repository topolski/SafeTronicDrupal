<?php

/**
 * @file
 * Korpa shopping cart implementation functions.
 */

/**
 * Callback function for cart listing.
 */
function korpa_cart() {
  $cart = korpa_get_cart();

  if (empty($cart)) {
    return t('Vaša korpa je prazna.');
  }
  return drupal_get_form('korpa_cart_form');
}

/**
 * Shopping cart form.
 */
function korpa_cart_form($form, &$form_state) {
  $form = array();  

  // Getting the shopping cart.
  $cart = korpa_get_cart();
  // And now the form.
  $form['cartcontents'] = array(
    // Make the returned array come back in tree form.
    '#tree' => TRUE,
    '#prefix' => '<div class="basic-cart-cart basic-cart-grid">',
    '#suffix' => '</div>',
  );
  // Cart elements.
  foreach ($cart as $nid => $node) {
    $form['cartcontents'][$nid] = array(
      '#type' => 'textfield',
      '#size' => 1,
      '#default_value' => $node->korpa_quantity,
      '#theme' => 'korpa_render_cart_element',
      '#ajax' => array(
        'callback' => 'update_callback',
        'wrapper' => 'tprice',
        'progress'=>array(
          'type'=>'none',
        ),
      ),
    );
  }
  if(!empty($form_state['values']['cartcontents'])){
    foreach ($form_state['values']['cartcontents'] as $nid => $value) {
      $quantity = (int) $value;
      if ($quantity >= 0) {
        $_SESSION['korpa']['cart'][$nid]->korpa_quantity = $quantity;
      }
    }
    korpa_get_total_price();
  }
  
  // Total price.
  $form['total_price'] = array(
    '#markup' => t('Ukupna cena'),
    '#prefix' => '<div id="tprice" class="basic-cart-cart basic-cart-grid">',
    '#suffix' => '</div>',
    '#theme' => 'korpa_cart_total_price',
  );
  
  // Buttons.
  $form['buttons'] = array(
    // Make the returned array come back in tree form.
    '#tree' => TRUE,
    '#prefix' => '<div class="row"><div class="basic-cart-call-to-action cell">',
    '#suffix' => '</div></div>',
  );
  
  // Update button.
  $form['buttons']['naruči'] = array(
    '#type' => 'button',
    '#value' => t('Naručite'),
    '#ajax' => array(
        'callback' => 'naruci_callback',
        'wrapper' => 'replace_fieldset_div',
        'progress'=>array(
          'type'=>'none',
        ),
    ),
  );
  
  $form['poruka'] = array(
    '#type' => 'markup',
    '#prefix'=>'<div id="replace_fieldset_div">',
    '#suffix'=>'</div>',
  );
  
  if(!empty($form_state['values']['buttons']['naruči'])){
    $form['poruka']['#markup'] = t('Porudžbina je uspela!!!');
  }

  return $form;
}

function update_callback($form, $form_state){
  return $form['total_price'];
}

function naruci_callback($form, $form_state){
  korpa_empty_cart();
  return $form['poruka'];
}

/**
 * Callback function for cart/add/.
 *
 * @param int $nid
 *   We are using the node id to store the node in the shopping cart
 */
function korpa_add_to_cart($nid = NULL, $quantity = 1) {
  $nid = (int) $nid;
  $quantity = (int) $quantity;
  if ($nid > 0 && $quantity > 0) {
    // If a node is added more times, just update the quantity.
    $cart = korpa_get_cart();
    if (!empty($cart) && in_array($nid, array_keys($cart))) {
      // Clicked 2 times on add to cart button. Increment quantity.
      $_SESSION['korpa']['cart'][$nid]->korpa_quantity += $quantity;
    }
    else {
      $node = node_load($nid);
      $node->korpa_quantity = $quantity;
      // Adding description.
      $body = field_get_items('node', $node, 'body');
      $description = isset($body[0]['value']) ? check_plain(strip_tags($body[0]['value'])) : '';
      $node->korpa_node_description = $description;
      // Unit price.
      $unit_price = field_get_items('node', $node, 'price');
      $unit_price = isset($unit_price[0]['value']) ? check_plain(strip_tags($unit_price[0]['value'])) : 0;
      $node->korpa_unit_price = $unit_price;
      
      // Storing in session.
      $_SESSION['korpa']['cart'][$nid] = $node;
    }
  }
}

/**
 * Function for shopping cart retrieval.
 *
 * @param int $nid
 *   We are using the node id to store the node in the shopping cart
 *
 * @return mixed
 *   Returning the shopping cart contents.
 *   An empty array if there is nothing in the cart
 */
function korpa_get_cart($nid = NULL) {
  if (isset($nid)) {
    return $_SESSION['korpa']['cart'][$nid];
  }
  if (isset($_SESSION['korpa']['cart'])) {
    return $_SESSION['korpa']['cart'];
  }
  // Empty cart.
  return array();
}

/**
 * Returns the final price for the shopping cart.
 *
 * @return mixed $total_price
 *   The total price for the shopping cart. 
 */
function korpa_get_total_price() {
  $currency = check_plain(variable_get('korpa_currency'));
  // Building the return array.
  $return = array(
    'price' => 0,
    'vat' => 0,
    'total' => 0,
  );
  $cart = korpa_get_cart();
  if (empty($cart)) {
    return (object) $return;
  }
  
  $total_price = 0;
  foreach ($cart as $nid => $node) {
    if (isset($node->korpa_quantity) && isset($node->korpa_unit_price)) {
      $total_price += $node->korpa_quantity * $node->korpa_unit_price;
    }
  }
  
  $return['price'] = $total_price.' '.$currency;
  $return['total'] = $total_price.' '.$currency;
  
  return (object) $return;
}

/**
 * Shopping cart reset.
 */
function korpa_empty_cart() {
  unset($_SESSION['korpa']['cart']);
}