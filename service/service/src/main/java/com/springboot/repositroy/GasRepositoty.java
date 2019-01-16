package com.springboot.repositroy;

import com.springboot.entities.Gas;
import com.springboot.entities.Guangzhao;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.stereotype.Repository;

import java.util.List;

/**
 * Created by 10497 on 2017/6/12.
 */
@Repository
public interface GasRepositoty extends JpaRepository<Gas,Integer>{
    @Query("select gas,date from Gas g ORDER BY  g.date desc ")
    List<Gas> findAllGas();
}
